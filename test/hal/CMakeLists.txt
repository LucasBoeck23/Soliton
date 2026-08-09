# test/hal/CMakeLists.txt — HAL unit and property tests

# ---------------------------------------------------------------------------
# Shared mock sources — used by multiple HAL property tests
# ---------------------------------------------------------------------------
set(HAL_MOCK_SOURCES
    ${SOLITON_ROOT}/components/hal/mock/src/hal_mock_ctrl.c
    ${SOLITON_ROOT}/components/hal/mock/src/hal_mock_i2c.c
    ${SOLITON_ROOT}/components/hal/mock/src/hal_mock_uart.c
    ${SOLITON_ROOT}/components/hal/mock/src/hal_mock_pwm.c
    ${SOLITON_ROOT}/components/hal/mock/src/hal_mock_adc.c
    ${SOLITON_ROOT}/components/hal/mock/src/hal_mock_gpio.c
)

# ---------------------------------------------------------------------------
# Test: HAL error code uniqueness (Property 2)
# ---------------------------------------------------------------------------
add_executable(test_error_codes
    test_error_codes.c
    ${SOLITON_ROOT}/components/hal/src/hal_error.c
)

target_include_directories(test_error_codes PRIVATE
    ${SOLITON_INCLUDE_DIRS}
)

target_link_libraries(test_error_codes PRIVATE unity)

add_test(NAME hal_error_code_uniqueness COMMAND test_error_codes)

# ---------------------------------------------------------------------------
# Test: Mock call recording (Property 1)
# ---------------------------------------------------------------------------
add_executable(test_mock_recording
    test_mock_recording.c
    ${HAL_MOCK_SOURCES}
    ${SOLITON_ROOT}/components/hal/src/hal_error.c
)

target_include_directories(test_mock_recording PRIVATE
    ${SOLITON_INCLUDE_DIRS}
)

target_link_libraries(test_mock_recording PRIVATE unity pbt_runner)

add_test(NAME hal_mock_call_recording COMMAND test_mock_recording)

# ---------------------------------------------------------------------------
# Test: Uninitiated peripheral guard (Property 4)
# ---------------------------------------------------------------------------
add_executable(test_not_init_guard
    test_not_init_guard.c
    ${HAL_MOCK_SOURCES}
    ${SOLITON_ROOT}/components/hal/src/hal_error.c
)

target_include_directories(test_not_init_guard PRIVATE
    ${SOLITON_INCLUDE_DIRS}
)

target_link_libraries(test_not_init_guard PRIVATE unity pbt_runner)

add_test(NAME hal_not_init_guard COMMAND test_not_init_guard)

# ---------------------------------------------------------------------------
# Test: Mock value injection round-trip (Property 6)
# ---------------------------------------------------------------------------
add_executable(test_mock_injection
    test_mock_injection.c
    ${HAL_MOCK_SOURCES}
    ${SOLITON_ROOT}/components/hal/src/hal_error.c
)

target_include_directories(test_mock_injection PRIVATE
    ${SOLITON_INCLUDE_DIRS}
)

target_link_libraries(test_mock_injection PRIVATE unity pbt_runner)

add_test(NAME hal_mock_injection_roundtrip COMMAND test_mock_injection)

# ---------------------------------------------------------------------------
# Test: Mock error simulation fidelity (Property 7)
# ---------------------------------------------------------------------------
add_executable(test_mock_error_sim
    test_mock_error_sim.c
    ${HAL_MOCK_SOURCES}
    ${SOLITON_ROOT}/components/hal/src/hal_error.c
)

target_include_directories(test_mock_error_sim PRIVATE
    ${SOLITON_INCLUDE_DIRS}
)

target_link_libraries(test_mock_error_sim PRIVATE unity pbt_runner)

add_test(NAME hal_mock_error_sim_fidelity COMMAND test_mock_error_sim)
