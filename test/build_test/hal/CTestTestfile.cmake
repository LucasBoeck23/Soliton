# CMake generated Testfile for 
# Source directory: C:/Git/Soliton/test/hal
# Build directory: C:/Git/Soliton/test/build_test/hal
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(hal_error_code_uniqueness "C:/Git/Soliton/test/build_test/hal/test_error_codes.exe")
set_tests_properties(hal_error_code_uniqueness PROPERTIES  _BACKTRACE_TRIPLES "C:/Git/Soliton/test/hal/CMakeLists.txt;29;add_test;C:/Git/Soliton/test/hal/CMakeLists.txt;0;")
add_test(hal_mock_call_recording "C:/Git/Soliton/test/build_test/hal/test_mock_recording.exe")
set_tests_properties(hal_mock_call_recording PROPERTIES  _BACKTRACE_TRIPLES "C:/Git/Soliton/test/hal/CMakeLists.txt;46;add_test;C:/Git/Soliton/test/hal/CMakeLists.txt;0;")
add_test(hal_not_init_guard "C:/Git/Soliton/test/build_test/hal/test_not_init_guard.exe")
set_tests_properties(hal_not_init_guard PROPERTIES  _BACKTRACE_TRIPLES "C:/Git/Soliton/test/hal/CMakeLists.txt;63;add_test;C:/Git/Soliton/test/hal/CMakeLists.txt;0;")
add_test(hal_mock_injection_roundtrip "C:/Git/Soliton/test/build_test/hal/test_mock_injection.exe")
set_tests_properties(hal_mock_injection_roundtrip PROPERTIES  _BACKTRACE_TRIPLES "C:/Git/Soliton/test/hal/CMakeLists.txt;80;add_test;C:/Git/Soliton/test/hal/CMakeLists.txt;0;")
add_test(hal_mock_error_sim_fidelity "C:/Git/Soliton/test/build_test/hal/test_mock_error_sim.exe")
set_tests_properties(hal_mock_error_sim_fidelity PROPERTIES  _BACKTRACE_TRIPLES "C:/Git/Soliton/test/hal/CMakeLists.txt;97;add_test;C:/Git/Soliton/test/hal/CMakeLists.txt;0;")
