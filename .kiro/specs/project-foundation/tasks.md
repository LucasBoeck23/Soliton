# Implementation Plan: Project Foundation

## Overview

Implementação da infraestrutura completa do projeto Sóliton: estrutura de diretórios ESP-IDF, sistema de build com CMake, camada HAL com interfaces para todos os periféricos, Mock Layer para testes em host, drivers stub para cada periférico, framework de testes Unity + PBT runner customizado, análise estática (clang-tidy/clang-format), validação de camadas e pipeline CI com GitHub Actions.

## Tasks

- [x] 1. Set up ESP-IDF project structure and build system
  - [x] 1.1 Create root project files and directory structure
    - Create root `CMakeLists.txt` with `project(soliton)` and ESP-IDF cmake inclusion
    - Create `sdkconfig.defaults` with ESP32-C3 target, flash size 4MB, crystal 40MHz, compiler optimization
    - Create directory tree: `components/hal/`, `components/drivers/`, `components/modules/`, `components/config/`, `components/utils/`, `main/`, `test/`, `docs/`, `tools/`, `hardware/`
    - Create `components/README.md` documenting each subdiretory responsibility and layer dependency rules
    - _Requirements: 1.1, 1.3, 1.5, 9.1, 9.2, 9.4_

  - [x] 1.2 Create main application entry point
    - Create `main/CMakeLists.txt` with `idf_component_register(SRCS "main.c" INCLUDE_DIRS ".")`
    - Create `main/main.c` with minimal `app_main()` stub that initializes and logs "Sóliton firmware started"
    - _Requirements: 1.1, 1.2_

  - [x] 1.3 Create config component with pinout definitions
    - Create `components/config/CMakeLists.txt` with `idf_component_register(INCLUDE_DIRS "include")` and `set(COMPONENT_LAYER "config")`
    - Create `components/config/include/soliton_pinout.h` with all pin definitions (I2C, PWM, UART, ADC, GPIO) as per design
    - _Requirements: 1.3, 9.2_

  - [x] 1.4 Create utils component placeholder
    - Create `components/utils/CMakeLists.txt` with `idf_component_register(SRCS "src/utils_placeholder.c" INCLUDE_DIRS "include")` and `set(COMPONENT_LAYER "utils")`
    - Create `components/utils/include/soliton_utils.h` with include guard `SOLITON_UTILS_SOLITON_UTILS_H`
    - Create `components/utils/src/utils_placeholder.c` minimal source
    - _Requirements: 1.3, 9.2_

- [x] 2. Implement HAL interfaces and error system
  - [x] 2.1 Create HAL error codes header
    - Create `components/hal/include/hal_error.h` with `hal_err_t` enum (HAL_OK=0, HAL_ERR_TIMEOUT=-1, HAL_ERR_INVALID_PARAM=-2, HAL_ERR_NOT_INIT=-3, HAL_ERR_COMM_FAIL=-4, HAL_ERR_BUSY=-5, HAL_ERR_NO_MEMORY=-6)
    - Implement `hal_err_to_str()` function declaration
    - Create `components/hal/src/hal_error.c` with `hal_err_to_str()` implementation
    - _Requirements: 3.4, 3.5_

  - [x] 2.2 Create HAL I2C interface
    - Create `components/hal/include/hal_i2c.h` with `hal_i2c_config_t` struct, `hal_i2c_init()`, `hal_i2c_write()`, `hal_i2c_read()`, `hal_i2c_deinit()` prototypes
    - Include guard: `SOLITON_HAL_I2C_H`
    - _Requirements: 3.1, 3.4_

  - [x] 2.3 Create HAL PWM interface
    - Create `components/hal/include/hal_pwm.h` with `hal_pwm_config_t` struct, `hal_pwm_init()`, `hal_pwm_set_duty()`, `hal_pwm_deinit()` prototypes
    - Include guard: `SOLITON_HAL_PWM_H`
    - _Requirements: 3.1, 3.4_

  - [x] 2.4 Create HAL UART interface
    - Create `components/hal/include/hal_uart.h` with `hal_uart_config_t` struct, `hal_uart_init()`, `hal_uart_write()`, `hal_uart_read()`, `hal_uart_deinit()` prototypes
    - Include guard: `SOLITON_HAL_UART_H`
    - _Requirements: 3.1, 3.4_

  - [x] 2.5 Create HAL ADC interface
    - Create `components/hal/include/hal_adc.h` with `hal_adc_config_t` struct, `hal_adc_init()`, `hal_adc_read_raw()`, `hal_adc_read_mv()`, `hal_adc_deinit()` prototypes
    - Include guard: `SOLITON_HAL_ADC_H`
    - _Requirements: 3.1, 3.4_

  - [x] 2.6 Create HAL GPIO interface
    - Create `components/hal/include/hal_gpio.h` with `hal_gpio_mode_t` enum, `hal_gpio_config_t` struct, `hal_gpio_init()`, `hal_gpio_set_level()`, `hal_gpio_get_level()`, `hal_gpio_deinit()` prototypes
    - Include guard: `SOLITON_HAL_GPIO_H`
    - _Requirements: 3.1, 3.4_

  - [x] 2.7 Create HAL component CMakeLists.txt
    - Create `components/hal/CMakeLists.txt` registering all HAL sources with `idf_component_register()`, INCLUDE_DIRS for `include` and conditional mock includes
    - Set `COMPONENT_LAYER "hal"`
    - Configure build flag `SOLITON_USE_MOCK` to select mock vs real implementation
    - _Requirements: 3.3, 1.3_

  - [x] 2.8 Write property test for HAL error code uniqueness
    - **Property 2: HAL error codes are unique**
    - Verify all `hal_err_t` values are distinct integers by iterating every pair
    - **Validates: Requirements 3.4**

- [x] 3. Implement Mock Layer for host testing
  - [x] 3.1 Create Mock control interface
    - Create `components/hal/mock/include/hal_mock_ctrl.h` with `hal_mock_call_t`, `hal_mock_history_t` structs, reset/injection/error simulation/query functions as per design
    - Include guard: `SOLITON_HAL_MOCK_CTRL_H`
    - _Requirements: 3.2, 5.7_

  - [x] 3.2 Implement Mock Layer core (history and control)
    - Create `components/hal/mock/src/hal_mock_ctrl.c` implementing call recording, history buffer, reset, injection setters, error simulation, and query functions
    - _Requirements: 3.2, 5.7, 5.8_

  - [x] 3.3 Implement Mock I2C
    - Create `components/hal/mock/src/hal_mock_i2c.c` implementing `hal_i2c_init/write/read/deinit` with call recording, configurable read data injection, init state tracking, and NOT_INIT guard
    - Default: returns HAL_OK, read returns injected data or zeros
    - _Requirements: 3.2, 3.6, 5.3, 5.9_

  - [x] 3.4 Implement Mock UART
    - Create `components/hal/mock/src/hal_mock_uart.c` implementing `hal_uart_init/write/read/deinit` with call recording, configurable read data, and init state tracking
    - Records commands sent via write for DFPlayer validation
    - _Requirements: 3.2, 5.4, 5.9_

  - [x] 3.5 Implement Mock PWM
    - Create `components/hal/mock/src/hal_mock_pwm.c` implementing `hal_pwm_init/set_duty/deinit` with call recording and duty-cycle value storage
    - Default duty: 0%
    - _Requirements: 3.2, 5.5, 5.9_

  - [x] 3.6 Implement Mock ADC
    - Create `components/hal/mock/src/hal_mock_adc.c` implementing `hal_adc_init/read_raw/read_mv/deinit` with call recording, configurable raw/voltage values
    - Default voltage: 3700mV
    - _Requirements: 3.2, 5.6, 5.9_

  - [x] 3.7 Implement Mock GPIO
    - Create `components/hal/mock/src/hal_mock_gpio.c` implementing `hal_gpio_init/set_level/get_level/deinit` with call recording and level storage
    - _Requirements: 3.2, 5.9_

  - [x] 3.8 Write property test for mock call recording
    - **Property 1: Mock call recording preserves call history**
    - Generate random sequences of HAL calls, verify history records every call in order with correct function names and parameters
    - **Validates: Requirements 3.2**

  - [x] 3.9 Write property test for uninitiated peripheral guard
    - **Property 4: Uninitiated peripheral guard**
    - For each peripheral type, generate random operations called before init, verify all return HAL_ERR_NOT_INIT
    - **Validates: Requirements 3.6**

  - [x] 3.10 Write property test for mock value injection round-trip
    - **Property 6: Mock value injection round-trip**
    - For each peripheral mock, inject random values within valid range, read through driver interface, verify exact match
    - **Validates: Requirements 5.3, 5.4, 5.5, 5.6, 5.7**

  - [x] 3.11 Write property test for mock error simulation fidelity
    - **Property 7: Mock error simulation fidelity**
    - Configure random error codes via `hal_mock_set_next_error()`, verify next call returns exact error, subsequent calls return HAL_OK
    - **Validates: Requirements 5.8**

- [x] 4. Checkpoint - Ensure HAL and Mock Layer compile and tests pass
  - Ensure all tests pass, ask the user if questions arise.

- [x] 5. Implement driver stubs
  - [x] 5.1 Implement VL53L0X driver stub
    - Create `components/drivers/vl53l0x/CMakeLists.txt` with `REQUIRES hal` and `set(COMPONENT_LAYER "drivers")`
    - Create `components/drivers/vl53l0x/include/drv_vl53l0x.h` with config struct, init/read_distance_mm/deinit prototypes, `VL53L0X_STUB_DISTANCE_MM = 500`
    - Create `components/drivers/vl53l0x/src/drv_vl53l0x.c` stub: init validates config and calls hal_i2c_init, read_distance_mm returns 500mm, proper NOT_INIT guard
    - _Requirements: 4.1, 4.2, 4.3, 4.5, 4.6, 4.7_

  - [x] 5.2 Implement DFPlayer driver stub
    - Create `components/drivers/dfplayer/CMakeLists.txt` with `REQUIRES hal` and `set(COMPONENT_LAYER "drivers")`
    - Create `components/drivers/dfplayer/include/drv_dfplayer.h` with config struct, init/play_track/set_volume/stop/deinit prototypes
    - Create `components/drivers/dfplayer/src/drv_dfplayer.c` stub: init calls hal_uart_init, play_track sends UART command, proper NOT_INIT guard
    - _Requirements: 4.1, 4.2, 4.3, 4.5, 4.6_

  - [x] 5.3 Implement Motor ERM driver stub
    - Create `components/drivers/motor_erm/CMakeLists.txt` with `REQUIRES hal` and `set(COMPONENT_LAYER "drivers")`
    - Create `components/drivers/motor_erm/include/drv_motor_erm.h` with config struct, init/set_duty/stop/deinit prototypes
    - Create `components/drivers/motor_erm/src/drv_motor_erm.c` stub: init calls hal_pwm_init, set_duty calls hal_pwm_set_duty, proper NOT_INIT guard
    - _Requirements: 4.1, 4.2, 4.3, 4.5, 4.6_

  - [x] 5.4 Implement Battery driver stub
    - Create `components/drivers/battery/CMakeLists.txt` with `REQUIRES hal` and `set(COMPONENT_LAYER "drivers")`
    - Create `components/drivers/battery/include/drv_battery.h` with config struct, init/read_voltage_mv/read_percent/deinit prototypes, `BATTERY_STUB_VOLTAGE_MV = 3700`
    - Create `components/drivers/battery/src/drv_battery.c` stub: init calls hal_adc_init, read_voltage_mv returns 3700mV, proper NOT_INIT guard
    - _Requirements: 4.1, 4.2, 4.3, 4.5, 4.6, 4.7_

  - [x] 5.5 Write property test for valid configuration initialization
    - **Property 5: Valid configuration initialization succeeds**
    - For each driver, generate random valid configurations (valid ports/channels/addresses), verify init returns HAL_OK
    - **Validates: Requirements 4.5**

  - [x] 5.6 Write property test for failed operations preserving peer state
    - **Property 3: Failed operations preserve peer peripheral state**
    - Initialize multiple peripherals, inject failure on one, verify others still respond correctly
    - **Validates: Requirements 3.5, 4.6**

  - [x] 5.7 Write unit tests for driver stubs
    - Test VL53L0X stub returns 500mm distance
    - Test Battery stub returns 3700mV voltage
    - Test all drivers return HAL_ERR_NOT_INIT when called before init
    - Test mock history records correct driver calls
    - _Requirements: 4.7, 4.6_

- [x] 6. Checkpoint - Ensure drivers compile and tests pass
  - Ensure all tests pass, ask the user if questions arise.

- [x] 7. Set up testing framework and PBT runner
  - [x] 7.1 Integrate Unity test framework
    - Add Unity as ESP-IDF component or submodule in `components/` or as managed dependency
    - Create `test/CMakeLists.txt` configuring test build for host target
    - Create test directory structure: `test/hal/`, `test/drivers/`, `test/modules/`, `test/utils/`, `test/build/`
    - _Requirements: 6.1, 6.3, 6.6_

  - [x] 7.2 Implement PBT runner utility
    - Create `test/utils/pbt_runner.h` with `pbt_context_t` struct, `pbt_init()`, `pbt_gen_uint32()`, `pbt_gen_uint8()`, `pbt_gen_int32()`, `PBT_RUN` macro (100 iterations)
    - Create `test/utils/pbt_runner.c` implementing PRNG seeded with time, generators for bounded ranges, seed reporting on failure
    - _Requirements: 6.1_

  - [x] 7.3 Configure CMock integration
    - Add CMock as dependency for auto-generating mocks from HAL headers during test build
    - Configure test build to link mock implementations when targeting host
    - _Requirements: 6.2, 6.4_

- [x] 8. Implement layer enforcement and static analysis
  - [x] 8.1 Create layer validation CMake script
    - Create `tools/check_layers.cmake` implementing `enforce_layer_rules()` function that validates dependency rules per layer
    - Integrate into root CMakeLists.txt to run at configure time
    - Emit `FATAL_ERROR` on violations with source component, forbidden target, and violated rule
    - _Requirements: 10.1, 10.2, 10.3, 10.4, 10.6_

  - [x] 8.2 Create .clang-format configuration
    - Create `.clang-format` with: BasedOnStyle LLVM, IndentWidth 4, ColumnLimit 100, BreakBeforeBraces Attach (K&R style)
    - _Requirements: 7.4_

  - [x] 8.3 Create .clang-tidy configuration
    - Create `.clang-tidy` with checks for naming conventions (snake_case functions/variables, UPPER_SNAKE_CASE constants), include guard format `SOLITON_*_H`
    - _Requirements: 7.1, 7.2, 7.5_

  - [x] 8.4 Write CMake test for layer dependency enforcement
    - **Property 8: Layer dependency enforcement**
    - Create `test/build/test_layer_rules.cmake` that tests various illegal dependency declarations trigger FATAL_ERROR with correct messaging
    - **Validates: Requirements 10.1, 10.2, 10.4**

  - [x] 8.5 Write CMake test for layer declaration required
    - **Property 9: Layer declaration required**
    - Create `test/build/test_layer_decl.cmake` that tests components without COMPONENT_LAYER trigger FATAL_ERROR
    - **Validates: Requirements 10.6**

- [x] 9. Set up CI pipeline and environment setup script
  - [x] 9.1 Create GitHub Actions CI workflow
    - Create `.github/workflows/ci.yml` with steps: checkout, cache ESP-IDF tools, static analysis (clang-format + clang-tidy), firmware build (`idf.py build`), unit tests (`idf.py -T test`)
    - Use `espressif/idf:v5.1` Docker image
    - Configure branch protection rules documentation for blocking merge on failure
    - _Requirements: 8.1, 8.2, 8.3, 8.4, 8.5, 8.6, 8.7_

  - [x] 9.2 Create environment setup script
    - Create `tools/setup.sh` for Linux/macOS that installs ESP-IDF v5.x, toolchain, Python deps, and validates with `idf.py --version`
    - Create `tools/setup.ps1` for Windows with equivalent functionality
    - Exit with non-zero code and descriptive message on failure
    - _Requirements: 2.1, 2.2, 2.3, 2.5_

  - [x] 9.3 Write comprehensive README with setup instructions
    - Document prerequisites for Linux (Ubuntu ≥ 22.04) and Windows (≥ 10)
    - Include numbered installation steps, minimum tool versions (Python ≥ 3.8, Git ≥ 2.30, CMake ≥ 3.16)
    - Document build commands, test execution, project structure overview
    - _Requirements: 2.4_

- [x] 10. Create component template and Wokwi emulation config
  - [x] 10.1 Create new component template
    - Create `tools/templates/component/CMakeLists.txt.template` with placeholder for SRCS, INCLUDE_DIRS, REQUIRES, and COMPONENT_LAYER
    - Create `tools/templates/component/include/template.h.template` with include guard pattern
    - Create `tools/templates/component/src/template.c.template` with minimal implementation
    - _Requirements: 9.3_

  - [x] 10.2 Create Wokwi emulation configuration
    - Create `hardware/wokwi/diagram.json` mapping ESP32-C3 with I2C (VL53L0X), UART (DFPlayer), PWM (motor), ADC (battery), GPIO (button)
    - _Requirements: 5.1, 5.2_

  - [x] 10.3 Create layer dependency diagram documentation
    - Create `docs/architecture.md` with Mermaid diagrams showing layer dependencies, allowed and forbidden directions
    - _Requirements: 10.5_

- [x] 11. Wire everything together and validate full build
  - [x] 11.1 Integrate all components in root CMakeLists.txt
    - Ensure root `CMakeLists.txt` includes all component directories
    - Verify `components/modules/CMakeLists.txt` placeholder exists with proper COMPONENT_LAYER
    - Ensure `idf.py build` compiles successfully with all components linked
    - Verify binary output `.bin` file is generated with size > 0
    - _Requirements: 1.1, 1.2, 1.4, 1.6_

  - [x] 11.2 Write integration tests for full build validation
    - Test `idf.py build` produces non-zero binary
    - Test `sdkconfig.defaults` contains required keys (CONFIG_IDF_TARGET, flash size, crystal freq)
    - Test include guards follow `SOLITON_*_H` pattern across all headers
    - Test CI YAML contains required steps in correct order
    - _Requirements: 1.1, 1.5, 7.5, 8.2_

- [x] 12. Final checkpoint - Ensure all tests pass
  - Ensure all tests pass, ask the user if questions arise.

## Notes

- Tasks marked with `*` are optional and can be skipped for faster MVP
- Each task references specific requirements for traceability
- Checkpoints ensure incremental validation
- Property tests validate universal correctness properties from the design document
- Unit tests validate specific examples and edge cases
- The design uses C/C++ with ESP-IDF framework — all code examples use C
- Layer enforcement is implemented at CMake configure time (compile-time check)
- Mock Layer is only linked when building for host target (tests), never for ESP32-C3 target

## Task Dependency Graph

```json
{
  "waves": [
    { "id": 0, "tasks": ["1.1", "1.3", "1.4"] },
    { "id": 1, "tasks": ["1.2", "2.1", "7.1"] },
    { "id": 2, "tasks": ["2.2", "2.3", "2.4", "2.5", "2.6"] },
    { "id": 3, "tasks": ["2.7", "2.8", "7.2", "7.3"] },
    { "id": 4, "tasks": ["3.1"] },
    { "id": 5, "tasks": ["3.2", "3.3", "3.4", "3.5", "3.6", "3.7"] },
    { "id": 6, "tasks": ["3.8", "3.9", "3.10", "3.11"] },
    { "id": 7, "tasks": ["5.1", "5.2", "5.3", "5.4"] },
    { "id": 8, "tasks": ["5.5", "5.6", "5.7"] },
    { "id": 9, "tasks": ["8.1", "8.2", "8.3"] },
    { "id": 10, "tasks": ["8.4", "8.5"] },
    { "id": 11, "tasks": ["9.1", "9.2", "9.3"] },
    { "id": 12, "tasks": ["10.1", "10.2", "10.3"] },
    { "id": 13, "tasks": ["11.1"] },
    { "id": 14, "tasks": ["11.2"] }
  ]
}
```
