# test/drivers/CMakeLists.txt — Driver unit and property tests

# ---------------------------------------------------------------------------
# Shared sources for driver tests
# ---------------------------------------------------------------------------
set(HAL_MOCK_SOURCES
    ${SOLITON_ROOT}/components/hal/mock/src/hal_mock_ctrl.c
    ${SOLITON_ROOT}/components/hal/mock/src/hal_mock_i2c.c
    ${SOLITON_ROOT}/components/hal/mock/src/hal_mock_uart.c
    ${SOLITON_ROOT}/components/hal/mock/src/hal_mock_pwm.c
    ${SOLITON_ROOT}/components/hal/mock/src/hal_mock_adc.c
    ${SOLITON_ROOT}/components/hal/mock/src/hal_mock_gpio.c
)

set(DRIVER_SOURCES
    ${SOLITON_ROOT}/components/drivers/vl53l0x/src/drv_vl53l0x.c
    ${SOLITON_ROOT}/components/drivers/dfplayer/src/drv_dfplayer.c
    ${SOLITON_ROOT}/components/drivers/motor_erm/src/drv_motor_erm.c
    ${SOLITON_ROOT}/components/drivers/battery/src/drv_battery.c
)

# ---------------------------------------------------------------------------
# Test: Valid config initialization (Property 5, Task 5.5)
# ---------------------------------------------------------------------------
add_executable(test_driver_init
    test_driver_init.c
    ${HAL_MOCK_SOURCES}
    ${DRIVER_SOURCES}
    ${SOLITON_ROOT}/components/hal/src/hal_error.c
)

target_include_directories(test_driver_init PRIVATE
    ${SOLITON_INCLUDE_DIRS}
)

target_link_libraries(test_driver_init PRIVATE unity pbt_runner)

add_test(NAME drivers_valid_config_init COMMAND test_driver_init)

# ---------------------------------------------------------------------------
# Test: Error state isolation (Property 3, Task 5.6)
# ---------------------------------------------------------------------------
add_executable(test_error_isolation
    test_error_isolation.c
    ${HAL_MOCK_SOURCES}
    ${DRIVER_SOURCES}
    ${SOLITON_ROOT}/components/hal/src/hal_error.c
)

target_include_directories(test_error_isolation PRIVATE
    ${SOLITON_INCLUDE_DIRS}
)

target_link_libraries(test_error_isolation PRIVATE unity pbt_runner)

add_test(NAME drivers_error_state_isolation COMMAND test_error_isolation)

# ---------------------------------------------------------------------------
# Test: Driver stubs unit tests (Task 5.7)
# ---------------------------------------------------------------------------
add_executable(test_driver_stubs
    test_driver_stubs.c
    ${HAL_MOCK_SOURCES}
    ${DRIVER_SOURCES}
    ${SOLITON_ROOT}/components/hal/src/hal_error.c
)

target_include_directories(test_driver_stubs PRIVATE
    ${SOLITON_INCLUDE_DIRS}
)

target_link_libraries(test_driver_stubs PRIVATE unity)

add_test(NAME driver_stubs COMMAND test_driver_stubs)
