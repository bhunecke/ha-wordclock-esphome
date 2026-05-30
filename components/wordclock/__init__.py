import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import time, light
from esphome.const import CONF_ID, CONF_TIME_ID

wordclock_ns = cg.esphome_ns.namespace('wordclock')
WordclockComponent = wordclock_ns.class_('Wordclock', cg.Component)

CONF_LIGHT_ID = "light_id"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(WordclockComponent),
    cv.Required(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
    cv.Required(CONF_LIGHT_ID): cv.use_id(light.LightState),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    cg.add_library("fastled/FastLED", "3.10.3")
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    time_ = await cg.get_variable(config[CONF_TIME_ID])
    cg.add(var.set_time(time_))
    
    light_ = await cg.get_variable(config[CONF_LIGHT_ID])
    cg.add(var.set_light(light_))
