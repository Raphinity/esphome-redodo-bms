#include "redodo_bms.h"

namespace esphome {
namespace redodo_bms {

static const char *TAG = "redodo_bms";

void RedodoBMS::update() {
   if (this->parent()->connected() != true){
    this->clear_sensor_values();
    return;
   }

   auto *chr = this->parent()->get_characteristic(SERVICE_UUID ,WRITE_CHAR_UUID);

   if (chr == nullptr) {
    ESP_LOGW(TAG, "Write characteristic not found");
    this->clear_sensor_values();
    return;
   }

   chr->write_value(cmd_query_battery_status, sizeof(cmd_query_battery_status), ESP_GATT_WRITE_TYPE_NO_RSP);

}

void RedodoBMS::gattc_event_handler(esp_gattc_cb_event_t event,
                                   esp_gatt_if_t gattc_if,
                                   esp_ble_gattc_cb_param_t *param) {
  switch (event) {

    case ESP_GATTC_CONNECT_EVT:
      ESP_LOGI(TAG, "BLE connected");
      break;

    case ESP_GATTC_DISCONNECT_EVT:
      ESP_LOGW(TAG, "BLE disconnected");
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
    }
    case ESP_GATTC_NOTIFY_EVT:
      ESP_LOGD(TAG, "Notification received (%d bytes)",param->notify.value_len);
      ESP_LOGD(TAG, "Hex: %s", format_hex_pretty(param->notify.value,param->notify.value_len).c_str());
      this->parse_notification_(param->notify.value,param->notify.value_len);
      break;

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
      read_u32(&pData[8]) / 1000.0f;

  this->bms_data.battery_current =
      (int32_t) read_u32(&pData[48]) / 1000.0f;

  // -----------------------------
  // Temperatures
  // -----------------------------
  this->bms_data.cell_temp =
      (int16_t)((pData[53] << 8) | pData[52]);

  this->bms_data.mosfet_temp =
      (int16_t)((pData[55] << 8) | pData[54]);

  // -----------------------------
  // Capacity
  // -----------------------------
  this->bms_data.battery_remaining_ah =
      ((pData[63] << 8) | pData[62]) / 100.0f;

  this->bms_data.battery_soc =
      ((pData[91] << 8) | pData[90]);

  // -----------------------------
  // Cell voltages → delta voltage
  // -----------------------------
  float min_v = 100.0f;
  float max_v = 0.0f;
  bool found = false;

  for (int i = 16; i < 48; i += 2) {
    uint16_t mv = (pData[i + 1] << 8) | pData[i];
    if (mv == 0)
      continue;

    float v = mv / 1000.0f;
    min_v = std::min(min_v, v);
    max_v = std::max(max_v, v);
    found = true;
  }

  this->bms_data.cell_delta_voltage = found ? (max_v - min_v) : NAN;

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

  set_sensor_values();
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
  
  if (this->cell_delta_voltage_sensor != nullptr) {
    this->cell_delta_voltage_sensor->publish_state(this->bms_data.cell_delta_voltage);
  }

  if (this->cell_temp_sensor != nullptr) {
    this->cell_temp_sensor->publish_state(this->bms_data.cell_temp);
  }

  if (this->mosfet_temp_sensor != nullptr) {
    this->mosfet_temp_sensor->publish_state(this->bms_data.mosfet_temp);
  }

  if (this->problem_binary_sensor != nullptr) {
    this->problem_binary_sensor->publish_state(this->bms_data.problem);
  }
  
  if (this->problem_binary_sensor != nullptr) {
    this->problem_description_text_sensor->publish_state(this->bms_data.problem_description);
  }
  
}

void RedodoBMS::clear_sensor_values(){
  this->bms_data.battery_voltage = NAN;
  this->bms_data.battery_current = NAN;
  this->bms_data.battery_remaining_ah = NAN;
  this->bms_data.cell_delta_voltage = NAN;
  this->bms_data.battery_soc = NAN;
  this->bms_data.mosfet_temp = NAN;
  this->bms_data.cell_temp = NAN;
  this->bms_data.problem = false;
  this->bms_data.problem_description = "";

  this->set_sensor_values();
}

}  // namespace redodo_bms
}  // namespace esphome

