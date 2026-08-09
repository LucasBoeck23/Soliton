# components/drivers/vl53l0x/CMakeLists.txt
# Driver VL53L0X — Sensor de distância laser Time-of-Flight (I2C)

idf_component_register(
    SRCS "src/drv_vl53l0x.c"
    INCLUDE_DIRS "include"
    REQUIRES hal
    PRIV_REQUIRES ""
)

# Layer enforcement: drivers só podem depender de hal e ESP-IDF
set(COMPONENT_LAYER "drivers")
