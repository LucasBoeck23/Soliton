# test/utils/CMakeLists.txt — Test utilities (PBT runner)

add_library(pbt_runner STATIC
    pbt_runner.c
)

target_include_directories(pbt_runner PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
