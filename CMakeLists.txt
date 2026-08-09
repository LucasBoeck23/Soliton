# Root CMakeLists.txt — Projeto Sóliton
cmake_minimum_required(VERSION 3.16)

# ---------------------------------------------------------------------------
# Layer dependency validation (runs at configure time)
# ---------------------------------------------------------------------------
include(${CMAKE_CURRENT_SOURCE_DIR}/tools/check_layers.cmake)

# ---------------------------------------------------------------------------
# Extra component directories for nested drivers
# ESP-IDF auto-discovers components/ but NOT subdirectories of subdirectories.
# Each driver under components/drivers/<name>/ must be explicitly listed.
# ---------------------------------------------------------------------------
set(EXTRA_COMPONENT_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/components/drivers/vl53l0x
    ${CMAKE_CURRENT_SOURCE_DIR}/components/drivers/dfplayer
    ${CMAKE_CURRENT_SOURCE_DIR}/components/drivers/motor_erm
    ${CMAKE_CURRENT_SOURCE_DIR}/components/drivers/battery
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(soliton)

# ---------------------------------------------------------------------------
# Post-project layer validation
# Validates all project components have COMPONENT_LAYER declared and
# their REQUIRES lists respect the layer dependency rules.
# ---------------------------------------------------------------------------
set(SOLITON_COMPONENTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/components)

# List of project components and their directories for validation
set(_soliton_components
    "hal;${SOLITON_COMPONENTS_DIR}/hal"
    "config;${SOLITON_COMPONENTS_DIR}/config"
    "utils;${SOLITON_COMPONENTS_DIR}/utils"
    "modules;${SOLITON_COMPONENTS_DIR}/modules"
    "vl53l0x;${SOLITON_COMPONENTS_DIR}/drivers/vl53l0x"
    "dfplayer;${SOLITON_COMPONENTS_DIR}/drivers/dfplayer"
    "motor_erm;${SOLITON_COMPONENTS_DIR}/drivers/motor_erm"
    "battery;${SOLITON_COMPONENTS_DIR}/drivers/battery"
)

# Check that every component declares COMPONENT_LAYER
foreach(_comp IN LISTS _soliton_components)
    string(REPLACE ";" ";" _parts "${_comp}")
    list(GET _parts 0 _name)
    list(GET _parts 1 _dir)
    check_layer_declaration(${_name} ${_dir})
endforeach()

# Validate layer rules for components with known REQUIRES
# (Extracted from each component's CMakeLists.txt declarations)
# drivers/ -> REQUIRES hal
enforce_layer_rules("vl53l0x" "drivers" "hal")
enforce_layer_rules("dfplayer" "drivers" "hal")
enforce_layer_rules("motor_erm" "drivers" "hal")
enforce_layer_rules("battery" "drivers" "hal")
