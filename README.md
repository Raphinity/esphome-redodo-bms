# ESPHome redodo_bms component

An ESPHome external component to read data from Redodo Battery Management Systems (BMS) via Bluetooth Low Energy (BLE)

Inspired by the following repos:
* https://github.com/mirosieber/Litime_BMS_ESP32
* https://github.com/calledit/LiTime_BMS_bluetooth

I tested this with a Redodo 24V 100Ah battery, but it may also work with Litime and PowerQueen batteries as the projects states above.

> [!WARNING]
> Do not use this integration for safety-critical applications. The data provided by this component is not based on an official protocol description and has not been validated by the manufacturer.

## Usage

```yaml
external_components:
  - source: github://csabaala/esphome-redodo-bms

ble_client:
  - mac_address: AA:BB:CC:DD:EE:FF
    id: ble_bms

redodo_bms:
    id: bms
    ble_client_id: ble_bms
    update_interval: 10s

sensor:
  - platform: redodo_bms
    redodo_bms_id: bms
    battery_voltage:
      name: "BMS Battery Voltage"
    battery_current:
      name: "BMS Battery Current"
    battery_soc:
      name: "BMS Battery SOC"
    battery_remaining_ah:
      name: "BMS Remaining Ah"
    cell_delta_voltage:
      name: "BMS Cell Delta Voltage"
    cell_temp:
      name: "BMS Temp Cell"
    mosfet_temp:
      name: "BMS Temp Mosfet"

text_sensor:
  - platform: redodo_bms
    redodo_bms_id: bms
    problem_description:
      name: "BMS Problem description"

binary_sensor:
  - platform: redodo_bms
    redodo_bms_id: bms
    problem:
      name: "BMS Problem"

```
