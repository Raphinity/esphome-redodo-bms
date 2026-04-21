import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_BATTERY,
    UNIT_VOLT,
    UNIT_AMPERE,
    UNIT_PERCENT,
    UNIT_CELSIUS,
    ICON_FLASH,
    ICON_PERCENT,
    ICON_THERMOMETER,
    ICON_BATTERY,
    STATE_CLASS_MEASUREMENT,
)

from . import redodo_bms_ns, RedodoBMS

CONF_REDODO_BMS_ID = "redodo_bms_id"

CONF_BATTERY_VOLTAGE = "battery_voltage"
CONF_BATTERY_CURRENT = "battery_current"
CONF_BATTERY_REMAINING_AH = "battery_remaining_ah"
CONF_DESIGN_CAPACITY = "design_capacity"
CONF_BATTERY_SOC = "battery_soc"
CONF_CELL_VOLTAGE_1 = "cell_voltage_1"
CONF_CELL_VOLTAGE_2 = "cell_voltage_2"
CONF_CELL_VOLTAGE_3 = "cell_voltage_3"
CONF_CELL_VOLTAGE_4 = "cell_voltage_4"
CONF_CELL_VOLTAGE_5 = "cell_voltage_5"
CONF_CELL_VOLTAGE_6 = "cell_voltage_6"
CONF_CELL_VOLTAGE_7 = "cell_voltage_7"
CONF_CELL_VOLTAGE_8 = "cell_voltage_8"
CONF_CELL_DELTA_VOLTAGE = "cell_delta_voltage"
CONF_CELL_TEMP = "cell_temp"
CONF_MOSFET_TEMP = "mosfet_temp"
CONF_BATTERY_HEALTH = "battery_health"
CONF_CYCLES = "cycles"
CONF_BALANCER = "balancer"

UNIT_AMPS_HOURS = "Ah"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_REDODO_BMS_ID): cv.use_id(RedodoBMS),
    cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        icon=ICON_FLASH,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_BATTERY_CURRENT): sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        icon=ICON_FLASH,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_BATTERY_SOC): sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        icon=ICON_BATTERY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
    ),
    cv.Optional(CONF_BATTERY_REMAINING_AH): sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPS_HOURS,
        icon=ICON_BATTERY,
        accuracy_decimals=0,
        device_class="",
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_DESIGN_CAPACITY): sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPS_HOURS,
        icon=ICON_BATTERY,
        accuracy_decimals=1,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_CELL_VOLTAGE_1): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        icon=ICON_FLASH,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_CELL_VOLTAGE_2): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        icon=ICON_FLASH,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_CELL_VOLTAGE_3): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        icon=ICON_FLASH,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_CELL_VOLTAGE_4): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        icon=ICON_FLASH,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_CELL_VOLTAGE_5): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        icon=ICON_FLASH,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_CELL_VOLTAGE_6): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        icon=ICON_FLASH,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_CELL_VOLTAGE_7): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        icon=ICON_FLASH,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_CELL_VOLTAGE_8): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        icon=ICON_FLASH,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_CELL_DELTA_VOLTAGE): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        icon=ICON_FLASH,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_CELL_TEMP): sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        icon=ICON_THERMOMETER,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_TEMPERATURE,
    ),
    cv.Optional(CONF_MOSFET_TEMP): sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        icon=ICON_THERMOMETER,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_TEMPERATURE,
    ),
cv.Optional(CONF_BATTERY_HEALTH): sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        icon=ICON_BATTERY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_CYCLES): sensor.sensor_schema(
        unit_of_measurement="",
        icon="mdi:recycle-variant",
        accuracy_decimals=0,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_BALANCER): sensor.sensor_schema(
        accuracy_decimals=0,
        icon="mdi:vector-selection",
    ),    
})

async def to_code(config):
    hub = await cg.get_variable(config[CONF_REDODO_BMS_ID])

    if CONF_BATTERY_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_VOLTAGE])
        cg.add(hub.set_battery_voltage_sensor(sens))

    if CONF_BATTERY_CURRENT in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_CURRENT])
        cg.add(hub.set_battery_current_sensor(sens))

    if CONF_BATTERY_SOC in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_SOC])
        cg.add(hub.set_battery_soc_sensor(sens))

    if CONF_BATTERY_REMAINING_AH in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_REMAINING_AH])
        cg.add(hub.set_battery_remaining_ah_sensor(sens))

    if CONF_DESIGN_CAPACITY in config:
        sens = await sensor.new_sensor(config[CONF_DESIGN_CAPACITY])
        cg.add(hub.set_design_capacity_sensor(sens))
    
    for i in range(1, 9):
        key = f"cell_voltage_{i}"
        if key in config:
            sens = await sensor.new_sensor(config[key])
            cg.add(hub.set_cell_voltage_sensor(i - 1, sens))

    if CONF_CELL_DELTA_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_CELL_DELTA_VOLTAGE])
        cg.add(hub.set_cell_delta_voltage_sensor(sens))

    if CONF_CELL_TEMP in config:
        sens = await sensor.new_sensor(config[CONF_CELL_TEMP])
        cg.add(hub.set_cell_temp_sensor(sens))

    if CONF_MOSFET_TEMP in config:
        sens = await sensor.new_sensor(config[CONF_MOSFET_TEMP])
        cg.add(hub.set_mosfet_temp_sensor(sens))

    if CONF_BATTERY_HEALTH in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_HEALTH])
        cg.add(hub.set_battery_health_sensor(sens))

    if CONF_CYCLES in config:
        sens = await sensor.new_sensor(config[CONF_CYCLES])
        cg.add(hub.set_cycles_sensor(sens))

    if CONF_BALANCER in config:
        sens = await sensor.new_sensor(config[CONF_BALANCER])
        cg.add(hub.set_balancer_sensor(sens))
