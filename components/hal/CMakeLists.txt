# components/hal/CMakeLists.txt
# Hardware Abstraction Layer — interfaces puras para periféricos
# Seleção de implementação: real (ESP32-C3) ou mock (host/testes)

set(HAL_SRCS "src/hal_error.c")

if(SOLITON_USE_MOCK)
    list(APPEND HAL_SRCS
        "mock/src/hal_mock_ctrl.c"
        "mock/src/hal_mock_i2c.c"
        "mock/src/hal_mock_uart.c"
        "mock/src/hal_mock_pwm.c"
        "mock/src/hal_mock_adc.c"
        "mock/src/hal_mock_gpio.c"
    )
    set(HAL_INCLUDE_DIRS "include" "mock/include")
else()
    set(HAL_INCLUDE_DIRS "include")
endif()

idf_component_register(
    SRCS ${HAL_SRCS}
    INCLUDE_DIRS ${HAL_INCLUDE_DIRS}
)

set(COMPONENT_LAYER "hal")
