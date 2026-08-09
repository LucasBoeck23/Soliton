# components/drivers/battery/CMakeLists.txt
# Driver Battery — Leitura de tensão e porcentagem da bateria (ADC)

idf_component_register(
    SRCS "src/drv_battery.c"
    INCLUDE_DIRS "include"
    REQUIRES hal
    PRIV_REQUIRES ""
)

# Layer enforcement: drivers só podem depender de hal e ESP-IDF
set(COMPONENT_LAYER "drivers")
