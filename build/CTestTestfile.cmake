# CMake generated Testfile for 
# Source directory: /home/abdullah/Desktop/BSCS/Semester -5/OS/Project/cJson
# Build directory: /home/abdullah/Desktop/BSCS/Semester -5/OS/Project/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(cJSON_test "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/build/cJSON_test")
set_tests_properties(cJSON_test PROPERTIES  _BACKTRACE_TRIPLES "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/cJson/CMakeLists.txt;248;add_test;/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/cJson/CMakeLists.txt;0;")
subdirs("tests")
subdirs("fuzzing")
