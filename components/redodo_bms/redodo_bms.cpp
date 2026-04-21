#include "redodo_bms.h"

namespace esphome {
namespace redodo_bms {

static const char *TAG = "redodo_bms";

void RedodoBMS::update() {
    if (!this->parent()->enabled) {
        ESP_LOGI(TAG, "Starting BLE client (Redodo BMS).");
        this->parent()->set_enabled(true); 
    } else if (this->parent()->connected()) {
        this->send_query_command_();
    }
}

void RedodoBMS::send_query_command_() {
   auto *chr = this->parent()->get_characteristic(SERVICE_UUID ,WRITE_CHAR_UUID);

   if (chr == nullptr) {
    ESP_LOGW(TAG, "Write characteristic not found");
    return;
   }

   ESP_LOGD(TAG, "Sending query command...");
   chr->write_value((uint8_t *)cmd_query_battery_status, sizeof(cmd_query_battery_status), ESP_GATT_WRITE_TYPE_NO_RSP);
}

void RedodoBMS::gattc_event_handler(esp_gattc_cb_event_t event,
                                   esp_gatt_if_t gattc_if,
                                   esp_ble_gattc_cb_param_t *param) {
  switch (event) {

    case ESP_GATTC_CONNECT_EVT:
      ESP_LOGI(TAG, "BLE connected");
      break;

    case ESP_GATTC_DISCONNECT_EVT:
      ESP_LOGI(TAG, "BLE disconnected");
      break;

    case ESP_GATTC_SEARCH_CMPL_EVT:{
      ESP_LOGI(TAG, "Service discovery complete");
      auto *chr = this->parent()->get_characteristic(SERVICE_UUID ,NOTIFY_CHAR_UUID);
       if (chr == nullptr) {
         ESP_LOGW(TAG, "Not found characteristic");
         this->clear_sensor_values();
         break;
       }
      esp_ble_gattc_register_for_notify(this->parent()->get_gattc_if(), this->parent()->get_remote_bda(), chr->handle);
      this->send_query_command_();
    }
    break;
    case ESP_GATTC_NOTIFY_EVT:{
      if (param->notify.handle == this->parent()->get_characteristic(SERVICE_UUID, NOTIFY_CHAR_UUID)->handle) {
        ESP_LOGD(TAG, "Notification received (%d bytes)",param->notify.value_len);
        ESP_LOGD(TAG, "Hex: %s", format_hex_pretty(param->notify.value,param->notify.value_len).c_str());
        this->parse_notification_(param->notify.value,param->notify.value_len);
      }
      break;
    }
    default:
      break;
  }
}

static uint32_t read_u32(const uint8_t *p) {
  return (uint32_t(p[3]) << 24) |
         (uint32_t(p[2]) << 16) |
         (uint32_t(p[1]) << 8)  |
         uint32_t(p[0]);
}

void RedodoBMS::parse_notification_(const uint8_t *pData, size_t length) {
  if (length < 104) {
    this->clear_sensor_values();
    ESP_LOGW(TAG, "Notification too short (%d bytes)", length);
    return;
  }

  // -----------------------------
  // Voltage & battery_current
  // -----------------------------
  this->bms_data.battery_voltage =
      (uint16_t)((pData[13] << 8) | pData[12]) / 1000.0f;

  this->bms_data.battery_current =
      (int32_t) read_u32(&pData[48]) / 1000.0f;

  // -----------------------------
  // Temperatures
  // -----------------------------
  this->bms_data.cell_temp = 
      (int16_t)((uint16_t)pData[53] << 8 | pData[52]);

  this->bms_data.mosfet_temp = 
      (int16_t)((uint16_t)pData[55] << 8 | pData[54]);

  // -----------------------------
  // Capacity
  // -----------------------------
  this->bms_data.battery_remaining_ah =
      (float)((uint16_t)pData[63] << 8 | pData[62]) / 100.0f;

  this->bms_data.design_capacity =
      (float)((uint16_t)pData[65] << 8 | pData[64]) / 100.0f;

  this->bms_data.battery_soc =
      (float)((uint16_t)pData[91] << 8 | pData[90]);

  // -----------------------------
  // Cell voltages → delta voltage
  // -----------------------------
  float min_v = 100.0f;
  float max_v = 0.0f;
  int cell_index = 0;
  bool found = false;

  for (int i = 16; i < 48 && cell_index < 8; i += 2) {
    uint16_t mv = (uint16_t)((uint16_t)pData[i + 1] << 8 | pData[i]);
    if (mv == 0)
      continue;

    float v = mv / 1000.0f;
    this->bms_data.cell_voltages[cell_index] = v;
    min_v = std::min(min_v, v);
    max_v = std::max(max_v, v);
    cell_index++;
    found = true;
  }

  this->bms_data.cell_delta_voltage = found ? (max_v - min_v) : NAN;

  // -----------------------------
  // Health
  // -----------------------------
  this->bms_data.battery_health =
      (float) read_u32(&pData[92]);

  // -----------------------------
  // Cycles
  // ----------------------------- 
  this->bms_data.cycles =
      (uint32_t) read_u32(&pData[96]);

  // -----------------------------
  // Balancer
  // ----------------------------- 
  this->bms_data.balancer_state =
    (uint32_t) read_u32(&pData[84]);

  // -----------------------------
  // Protection / problem flags
  // -----------------------------
  uint32_t protection =
      read_u32(&pData[76]);

  this->bms_data.problem = (protection != 0);

  if (this->bms_data.problem) {
    char buf[64];
    snprintf(buf, sizeof(buf),
             "Flags: 0x%08X", protection);
    this->bms_data.problem_description = buf;
  } else {
    this->bms_data.problem_description = "OK";
  }

  // -----------------------------
  // Debug summary
  // -----------------------------
  ESP_LOGD(TAG,
           "V=%.2fV I=%.2fA ΔV=%.3fV SOC=%.0f Problem=%d",
           this->bms_data.battery_voltage,
           this->bms_data.battery_current,
           this->bms_data.cell_delta_voltage,
           this->bms_data.battery_soc,
           this->bms_data.problem);

  this->set_sensor_values();

  this->set_timeout("finish_cycle", 1000, [this]() {
        ESP_LOGI(TAG, "Data processed. Stopping BLE client (Redodo BMS).");
        this->parent()->set_enabled(false);
    });
}

void RedodoBMS::set_sensor_values(){
  if (this->battery_voltage_sensor != nullptr) {
    this->battery_voltage_sensor->publish_state(this->bms_data.battery_voltage);
  }

  if (this->battery_current_sensor != nullptr) {
    this->battery_current_sensor->publish_state(this->bms_data.battery_current);
  }
  
  if (this->battery_soc_sensor != nullptr) {
    this->battery_soc_sensor->publish_state(this->bms_data.battery_soc);
  }

  if (this->battery_remaining_ah_sensor != nullptr) {
    this->battery_remaining_ah_sensor->publish_state(this->bms_data.battery_remaining_ah);
  }

  if (this->design_capacity_sensor != nullptr) {
    this->design_capacity_sensor->publish_state(this->bms_data.design_capacity);
  }

  for (int i = 0; i < 8; i++) {
    if (this->cell_voltages_sensors[i] != nullptr) {
      this->cell_voltages_sensors[i]->publish_state(this->bms_data.cell_voltages[i]);
    }
  }

  if (this->cell_delta_voltage_sensor != nullptr) {
    this->cell_delta_voltage_sensor->publish_state(this->bms_data.cell_delta_voltage);
  }

  if (this->cell_temp_sensor != nullptr) {
    this->cell_temp_sensor->publish_state(this->bms_data.cell_temp);
  }

  if (this->mosfet_temp_sensor != nullptr) {
    this->mosfet_temp_sensor->publish_state(this->bms_data.mosfet_temp);
  }

  if (this->battery_health_sensor != nullptr) {
    this->battery_health_sensor->publish_state(this->bms_data.battery_health);
  }

  if (this->cycles_sensor != nullptr) {
    this->cycles_sensor->publish_state(this->bms_data.cycles);
  }
 
  if (this->balancer_sensor != nullptr) {
    this->balancer_sensor->publish_state(this->bms_data.balancer_state);
  }

  if (this->problem_binary_sensor != nullptr) {
    this->problem_binary_sensor->publish_state(this->bms_data.problem);
  }
  
  if (this->problem_description_text_sensor != nullptr) {
    this->problem_description_text_sensor->publish_state(this->bms_data.problem_description);
  }
  
}

void RedodoBMS::clear_sensor_values(){
  this->bms_data.battery_voltage = NAN;
  this->bms_data.battery_current = NAN;
  this->bms_data.battery_remaining_ah = NAN;
  this->bms_data.design_capacity = NAN;
  this->bms_data.cell_delta_voltage = NAN;
  this->bms_data.battery_soc = NAN;
  this->bms_data.mosfet_temp = NAN;
  this->bms_data.cell_temp = NAN;
  this->bms_data.battery_health = NAN;
  this->bms_data.cycles = 0;
  this->bms_data.balancer_state = 0;
  this->bms_data.problem = false;
  this->bms_data.problem_description = "";

  this->set_sensor_values();
}

}  // namespace redodo_bms
}  // namespace esphome
