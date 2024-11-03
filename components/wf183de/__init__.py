# Required for ESPHome external component recognition
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import i2c
from esphome.const import CONF_ID, CONF_SIZE

DEPENDENCIES = ["i2c"]
MULTI_CONF = True
