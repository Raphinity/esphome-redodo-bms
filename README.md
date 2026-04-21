# ESPHome redodo_bms component

An ESPHome external component to read data from Redodo (also LiTime and Power Queen) Battery Management Systems (BMS) via Bluetooth Low Energy (BLE).

This is a fork of [csabaala/esphome-redodo-bms](https://github.com/csabaala/esphome-redodo-bms) featuring automated GATT connection cycles. It connects to the BMS only during update intervals, reducing resource usage on both the ESP32 and BMS while allowing other devices to connect between updates.

Inspired by the following repo : [patman15/aiobmsble](https://github.com/patman15/aiobmsble).

> [!WARNING]
> Do not use this integration for safety-critical applications. The data provided by this component is not based on an official protocol description and has not been validated by the manufacturer.

## Usage

```yaml
external_components:
  - source: github://Raphinity/esphome-redodo-bms

ble_client:
  - mac_address: AA:BB:CC:DD:EE:FF
    id: ble_bms

redodo_bms:
    id: bms
    ble_client_id: ble_bms
    update_interval: 300s

sensor:
  - platform: redodo_bms
    redodo_bms_id: bms
    battery_voltage:
      name: "BMS Battery Voltage"
      expire_after: 900s
    battery_current:
      name: "BMS Battery Current"
      expire_after: 900s
    battery_soc:
      name: "BMS Battery SOC"
      expire_after: 900s
    battery_remaining_ah:
      name: "BMS Remaining Ah"
      expire_after: 900s
    design_capacity:
      name: "BMS Design Capacity"
      expire_after: 900s
    cell_voltage_1:
      name: "BMS Cell 1 Voltage"
      expire_after: 900s
    cell_voltage_2:
      name: "BMS Cell 2 Voltage"
      expire_after: 900s
    cell_voltage_3:
      name: "BMS Cell 3 Voltage"
      expire_after: 900s 
    cell_voltage_4:
      name: "BMS Cell 4 Voltage"
      expire_after: 900s
    cell_delta_voltage:
      name: "BMS Cell Delta Voltage"
      expire_after: 900s
    cell_temp:
      name: "BMS Temp Cell"
      expire_after: 900s
    mosfet_temp:
      name: "BMS Temp Mosfet"
      expire_after: 900s
    battery_health:
      name: "BMS Battery Health"
      expire_after: 900s
    cycles:
      name: "BMS Charge Cycles"
      expire_after: 900s
    balancer:
      name: "BMS Balancer State"
      expire_after: 900s

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
