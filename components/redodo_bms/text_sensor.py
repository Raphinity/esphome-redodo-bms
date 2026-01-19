import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor

from . import redodo_bms_ns, RedodoBMS

CONF_REDODO_BMS_ID = "redodo_bms_id"

CONF_PROBLEM_DESCRIPTION = "problem_description"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_REDODO_BMS_ID): cv.use_id(RedodoBMS),
    cv.Required(CONF_REDODO_BMS_ID): cv.use_id(RedodoBMS),
    cv.Optional(CONF_PROBLEM_DESCRIPTION): text_sensor.text_sensor_schema(),
})

async def to_code(config):
    hub = await cg.get_variable(config[CONF_REDODO_BMS_ID])

    if CONF_PROBLEM_DESCRIPTION in config:
        ts = await text_sensor.new_text_sensor(config[CONF_PROBLEM_DESCRIPTION])
        cg.add(hub.set_problem_description_text_sensor(ts))

