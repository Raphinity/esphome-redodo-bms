#pragma once
#include "esphome/core/component.h"
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace redodo_bms {

static const auto SERVICE_UUID = esphome::esp32_ble::ESPBTUUID::from_raw("0000ffe0-0000-1000-8000-00805f9b34fb");
static const auto NOTIFY_CHAR_UUID = esphome::esp32_ble::ESPBTUUID::from_raw("0000ffe1-0000-1000-8000-00805f9b34fb");
static const auto WRITE_CHAR_UUID = esphome::esp32_ble::ESPBTUUID::from_raw("0000ffe2-0000-1000-8000-00805f9b34fb");

static uint8_t cmd_query_battery_status[] = {0x00, 0x00, 0x04, 0x01, 0x13, 0x55, 0xAA, 0x17};

struct RedodoBMSData {
  float battery_voltage = NAN;
  float battery_current = NAN;
  float battery_remaining_ah = NAN;
  float cell_delta_voltage = NAN;
  float battery_soc = NAN;
  float mosfet_temp = NAN;
  float cell_temp = NAN;
  bool problem = false;
  std::string problem_description;
  
};

class RedodoBMS : public PollingComponent, public ble_client::BLEClientNode  {
 public:
  void update() override;
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) override;
  void parse_notification_(const uint8_t *pData, size_t length);
  void set_sensor_values();
  void clear_sensor_values();

  void set_battery_voltage_sensor(sensor::Sensor *sensor) { this->battery_voltage_sensor = sensor; }
  void set_battery_current_sensor(sensor::Sensor *sensor) { this->battery_current_sensor = sensor; }
  void set_battery_soc_sensor(sensor::Sensor *sensor) { this->battery_soc_sensor = sensor; }
  void set_battery_remaining_ah_sensor(sensor::Sensor *sensor) { this->battery_remaining_ah_sensor = sensor; }
  void set_cell_delta_voltage_sensor(sensor::Sensor *sensor) { this->cell_delta_voltage_sensor = sensor; }
  void set_cell_temp_sensor(sensor::Sensor *sensor) { this->cell_temp_sensor = sensor; }
  void set_mosfet_temp_sensor(sensor::Sensor *sensor) { this->mosfet_temp_sensor = sensor; }
  
  void set_problem_binary_sensor(binary_sensor::BinarySensor *binary_sensor) { this->problem_binary_sensor = binary_sensor; }

  void set_problem_description_text_sensor(text_sensor::TextSensor *text_sensor) { this->problem_description_text_sensor = text_sensor; }
 
 protected:
  RedodoBMSData bms_data;
  
  sensor::Sensor *battery_voltage_sensor{nullptr};
  sensor::Sensor *battery_current_sensor{nullptr};
  sensor::Sensor *battery_soc_sensor{nullptr};
  sensor::Sensor *battery_remaining_ah_sensor{nullptr};
  sensor::Sensor *cell_delta_voltage_sensor{nullptr};
  sensor::Sensor *cell_temp_sensor{nullptr};
  sensor::Sensor *mosfet_temp_sensor{nullptr};

  binary_sensor::BinarySensor *problem_binary_sensor{nullptr};

  text_sensor::TextSensor *problem_description_text_sensor{nullptr};

};


}  // namespace redodo_bms
}  // namespace esphome

