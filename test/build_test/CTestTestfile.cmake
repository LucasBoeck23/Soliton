# CMake generated Testfile for 
# Source directory: C:/Git/Soliton/test
# Build directory: C:/Git/Soliton/test/build_test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(property8_layer_enforcement "C:/Program Files/CMake/bin/cmake.exe" "-P" "C:/Git/Soliton/test/build/test_layer_rules.cmake")
set_tests_properties(property8_layer_enforcement PROPERTIES  LABELS "build;layers;property" _BACKTRACE_TRIPLES "C:/Git/Soliton/test/CMakeLists.txt;125;add_test;C:/Git/Soliton/test/CMakeLists.txt;0;")
add_test(property9_layer_declaration "C:/Program Files/CMake/bin/cmake.exe" "-P" "C:/Git/Soliton/test/build/test_layer_decl.cmake")
set_tests_properties(property9_layer_declaration PROPERTIES  LABELS "build;layers;property" _BACKTRACE_TRIPLES "C:/Git/Soliton/test/CMakeLists.txt;135;add_test;C:/Git/Soliton/test/CMakeLists.txt;0;")
subdirs("_deps/unity-build")
subdirs("utils")
subdirs("hal")
subdirs("drivers")
subdirs("modules")
