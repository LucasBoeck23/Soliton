# CMake generated Testfile for 
# Source directory: C:/Git/Soliton/test/drivers
# Build directory: C:/Git/Soliton/test/build_test/drivers
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(drivers_valid_config_init "C:/Git/Soliton/test/build_test/drivers/test_driver_init.exe")
set_tests_properties(drivers_valid_config_init PROPERTIES  _BACKTRACE_TRIPLES "C:/Git/Soliton/test/drivers/CMakeLists.txt;38;add_test;C:/Git/Soliton/test/drivers/CMakeLists.txt;0;")
add_test(drivers_error_state_isolation "C:/Git/Soliton/test/build_test/drivers/test_error_isolation.exe")
set_tests_properties(drivers_error_state_isolation PROPERTIES  _BACKTRACE_TRIPLES "C:/Git/Soliton/test/drivers/CMakeLists.txt;56;add_test;C:/Git/Soliton/test/drivers/CMakeLists.txt;0;")
add_test(driver_stubs "C:/Git/Soliton/test/build_test/drivers/test_driver_stubs.exe")
set_tests_properties(driver_stubs PROPERTIES  _BACKTRACE_TRIPLES "C:/Git/Soliton/test/drivers/CMakeLists.txt;74;add_test;C:/Git/Soliton/test/drivers/CMakeLists.txt;0;")
