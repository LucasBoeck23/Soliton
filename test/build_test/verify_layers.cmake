# Verification script for tools/check_layers.cmake
# Run with: cmake -P test/build_test/verify_layers.cmake
# This validates the layer enforcement logic works correctly.

cmake_minimum_required(VERSION 3.16)

# Include the layer validation script
include(${CMAKE_CURRENT_LIST_DIR}/../../tools/check_layers.cmake)

# ---------------------------------------------------------------------------
# Test get_component_layer()
# ---------------------------------------------------------------------------
message(STATUS "Testing get_component_layer()...")

get_component_layer("hal" _layer)
if(NOT _layer STREQUAL "hal")
    message(FATAL_ERROR "FAIL: Expected 'hal' but got '${_layer}'")
endif()

get_component_layer("vl53l0x" _layer)
if(NOT _layer STREQUAL "drivers")
    message(FATAL_ERROR "FAIL: Expected 'drivers' but got '${_layer}'")
endif()

get_component_layer("dfplayer" _layer)
if(NOT _layer STREQUAL "drivers")
    message(FATAL_ERROR "FAIL: Expected 'drivers' but got '${_layer}'")
endif()

get_component_layer("motor_erm" _layer)
if(NOT _layer STREQUAL "drivers")
    message(FATAL_ERROR "FAIL: Expected 'drivers' but got '${_layer}'")
endif()

get_component_layer("battery" _layer)
if(NOT _layer STREQUAL "drivers")
    message(FATAL_ERROR "FAIL: Expected 'drivers' but got '${_layer}'")
endif()

get_component_layer("modules" _layer)
if(NOT _layer STREQUAL "modules")
    message(FATAL_ERROR "FAIL: Expected 'modules' but got '${_layer}'")
endif()

get_component_layer("utils" _layer)
if(NOT _layer STREQUAL "utils")
    message(FATAL_ERROR "FAIL: Expected 'utils' but got '${_layer}'")
endif()

get_component_layer("config" _layer)
if(NOT _layer STREQUAL "config")
    message(FATAL_ERROR "FAIL: Expected 'config' but got '${_layer}'")
endif()

get_component_layer("freertos" _layer)
if(NOT _layer STREQUAL "external")
    message(FATAL_ERROR "FAIL: Expected 'external' but got '${_layer}'")
endif()

message(STATUS "  get_component_layer() — ALL PASSED")

# ---------------------------------------------------------------------------
# Test enforce_layer_rules() — valid dependencies (should NOT error)
# ---------------------------------------------------------------------------
message(STATUS "Testing enforce_layer_rules() — valid dependencies...")

# modules can depend on hal, utils, config
enforce_layer_rules("test_module" "modules" "hal;utils;config")

# drivers can depend on hal
enforce_layer_rules("test_driver" "drivers" "hal")

# External deps are always OK
enforce_layer_rules("test_module" "modules" "freertos;esp_system")
enforce_layer_rules("test_driver" "drivers" "hal;esp_timer")

message(STATUS "  Valid dependencies — ALL PASSED")

# ---------------------------------------------------------------------------
# Test enforce_layer_rules() — invalid dependencies (each should FATAL_ERROR)
# We use execute_process to run sub-cmake scripts that should fail.
# ---------------------------------------------------------------------------
message(STATUS "Testing enforce_layer_rules() — violations...")

# Helper: run a cmake script and expect it to fail
function(expect_fatal_error test_name script_content)
    # Write a temporary script
    set(_tmp_script "${CMAKE_CURRENT_LIST_DIR}/_tmp_violation_test.cmake")
    file(WRITE "${_tmp_script}" 
        "include(${CMAKE_CURRENT_LIST_DIR}/../../tools/check_layers.cmake)\n${script_content}\n")
    
    execute_process(
        COMMAND ${CMAKE_COMMAND} -P "${_tmp_script}"
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE _output
        ERROR_VARIABLE _error
    )
    
    file(REMOVE "${_tmp_script}")
    
    if(_result EQUAL 0)
        message(FATAL_ERROR "FAIL [${test_name}]: Expected FATAL_ERROR but cmake succeeded")
    endif()
    
    # Check that error message contains "LAYER VIOLATION"
    string(FIND "${_error}" "LAYER VIOLATION" _found_pos)
    if(_found_pos EQUAL -1)
        message(FATAL_ERROR "FAIL [${test_name}]: Error message doesn't contain 'LAYER VIOLATION': ${_error}")
    endif()
    
    message(STATUS "  PASS: ${test_name}")
endfunction()

# modules -> drivers (FORBIDDEN)
expect_fatal_error("modules cannot depend on drivers"
    "enforce_layer_rules(\"radar\" \"modules\" \"vl53l0x\")")

# drivers -> modules (FORBIDDEN)
expect_fatal_error("drivers cannot depend on modules"
    "enforce_layer_rules(\"vl53l0x\" \"drivers\" \"modules\")")

# drivers -> utils (FORBIDDEN)
expect_fatal_error("drivers cannot depend on utils"
    "enforce_layer_rules(\"vl53l0x\" \"drivers\" \"utils\")")

# drivers -> other drivers (FORBIDDEN)
expect_fatal_error("drivers cannot depend on other drivers"
    "enforce_layer_rules(\"vl53l0x\" \"drivers\" \"dfplayer\")")

# hal -> drivers (FORBIDDEN)
expect_fatal_error("hal cannot depend on drivers"
    "enforce_layer_rules(\"hal\" \"hal\" \"vl53l0x\")")

# hal -> modules (FORBIDDEN)
expect_fatal_error("hal cannot depend on modules"
    "enforce_layer_rules(\"hal\" \"hal\" \"modules\")")

# utils -> hal (FORBIDDEN)
expect_fatal_error("utils cannot depend on hal"
    "enforce_layer_rules(\"utils\" \"utils\" \"hal\")")

# utils -> drivers (FORBIDDEN)
expect_fatal_error("utils cannot depend on drivers"
    "enforce_layer_rules(\"utils\" \"utils\" \"vl53l0x\")")

# utils -> modules (FORBIDDEN)
expect_fatal_error("utils cannot depend on modules"
    "enforce_layer_rules(\"utils\" \"utils\" \"modules\")")

# config -> any project component (FORBIDDEN)
expect_fatal_error("config cannot depend on hal"
    "enforce_layer_rules(\"config\" \"config\" \"hal\")")

message(STATUS "  Layer violation detection — ALL PASSED")

# ---------------------------------------------------------------------------
# Summary
# ---------------------------------------------------------------------------
message(STATUS "")
message(STATUS "=============================================")
message(STATUS "  check_layers.cmake — ALL TESTS PASSED ✓")
message(STATUS "=============================================")
