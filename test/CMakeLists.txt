# test/CMakeLists.txt — Host-based test build for Sóliton project
# This build does NOT use ESP-IDF. It compiles tests natively on the host
# using a standard C compiler (GCC, Clang, or MSVC).

cmake_minimum_required(VERSION 3.16)
project(soliton_tests C)

enable_testing()

# ---------------------------------------------------------------------------
# Fetch Unity test framework (ThrowTheSwitch/Unity v2.5.x)
# ---------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.5.2
)
FetchContent_MakeAvailable(unity)

# Create Unity library target if not already defined by the fetched project
if(NOT TARGET unity)
    add_library(unity STATIC
        ${unity_SOURCE_DIR}/src/unity.c
    )
    target_include_directories(unity PUBLIC
        ${unity_SOURCE_DIR}/src
    )
endif()

# ---------------------------------------------------------------------------
# Fetch CMock for auto-generating mocks from HAL headers (optional)
# CMock includes Unity as a dependency, but we manage Unity separately above.
# Primary mock strategy uses manual Mock Layer (components/hal/mock/).
# CMock provides optional auto-generation from headers via Ruby scripts.
# ---------------------------------------------------------------------------
FetchContent_Declare(
    cmock
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/CMock.git
    GIT_TAG        v2.5.2
)
FetchContent_MakeAvailable(cmock)

set(CMOCK_DIR ${cmock_SOURCE_DIR})
set(CMOCK_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/cmock_config.yml)

# CMock Ruby script path (used for optional auto-mock generation)
set(CMOCK_SCRIPT ${CMOCK_DIR}/lib/cmock.rb)

# ---------------------------------------------------------------------------
# CMock auto-generation function (optional — called per-header as needed)
# Usage: cmock_generate_mock(<header_path> <output_dir>)
# Requires Ruby installed on the host system.
# ---------------------------------------------------------------------------
function(cmock_generate_mock HEADER_PATH OUTPUT_DIR)
    get_filename_component(HEADER_NAME ${HEADER_PATH} NAME_WE)
    set(MOCK_SRC "${OUTPUT_DIR}/mock_${HEADER_NAME}.c")
    set(MOCK_HDR "${OUTPUT_DIR}/mock_${HEADER_NAME}.h")

    add_custom_command(
        OUTPUT ${MOCK_SRC} ${MOCK_HDR}
        COMMAND ruby ${CMOCK_SCRIPT} -o${CMOCK_CONFIG} ${HEADER_PATH}
        DEPENDS ${HEADER_PATH} ${CMOCK_CONFIG}
        COMMENT "CMock: Generating mock for ${HEADER_NAME}"
        WORKING_DIRECTORY ${OUTPUT_DIR}
    )
endfunction()

# ---------------------------------------------------------------------------
# Include directories for component headers (used by tests)
# ---------------------------------------------------------------------------
set(SOLITON_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Output directory for CMock-generated mocks
set(CMOCK_MOCK_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/cmock_mocks)
file(MAKE_DIRECTORY ${CMOCK_MOCK_OUTPUT_DIR})

set(SOLITON_INCLUDE_DIRS
    ${SOLITON_ROOT}/components/hal/include
    ${SOLITON_ROOT}/components/hal/mock/include
    ${SOLITON_ROOT}/components/drivers/vl53l0x/include
    ${SOLITON_ROOT}/components/drivers/dfplayer/include
    ${SOLITON_ROOT}/components/drivers/motor_erm/include
    ${SOLITON_ROOT}/components/drivers/battery/include
    ${SOLITON_ROOT}/components/config/include
    ${SOLITON_ROOT}/components/utils/include
    ${CMOCK_MOCK_OUTPUT_DIR}
    ${CMOCK_DIR}/src
)

# ---------------------------------------------------------------------------
# Compile definitions for mock/host builds
# ---------------------------------------------------------------------------
add_compile_definitions(SOLITON_USE_MOCK=1)

# ---------------------------------------------------------------------------
# Test utilities (PBT runner, etc.)
# ---------------------------------------------------------------------------
# Will be populated by task 7.2
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/utils/CMakeLists.txt)
    add_subdirectory(utils)
endif()

# ---------------------------------------------------------------------------
# Test subdirectories
# ---------------------------------------------------------------------------
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/hal/CMakeLists.txt)
    add_subdirectory(hal)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/drivers/CMakeLists.txt)
    add_subdirectory(drivers)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/modules/CMakeLists.txt)
    add_subdirectory(modules)
endif()

# ---------------------------------------------------------------------------
# Build/CMake script tests (run via cmake -P, registered with CTest)
# These test the build system logic itself (layer enforcement, etc.)
# ---------------------------------------------------------------------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/test_layer_rules.cmake")
    add_test(
        NAME "property8_layer_enforcement"
        COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_SOURCE_DIR}/build/test_layer_rules.cmake"
    )
    set_tests_properties("property8_layer_enforcement" PROPERTIES
        LABELS "build;layers;property"
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/test_layer_decl.cmake")
    add_test(
        NAME "property9_layer_declaration"
        COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_SOURCE_DIR}/build/test_layer_decl.cmake"
    )
    set_tests_properties("property9_layer_declaration" PROPERTIES
        LABELS "build;layers;property"
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/test_build_validation.cmake")
    add_test(
        NAME "integration_build_validation"
        COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_SOURCE_DIR}/build/test_build_validation.cmake"
    )
    set_tests_properties("integration_build_validation" PROPERTIES
        LABELS "build;integration"
    )
endif()
