# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/cJson"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/build"

# Include any dependencies generated for this target.
include tests/CMakeFiles/print_object.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tests/CMakeFiles/print_object.dir/compiler_depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/print_object.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/print_object.dir/flags.make

tests/CMakeFiles/print_object.dir/print_object.c.o: tests/CMakeFiles/print_object.dir/flags.make
tests/CMakeFiles/print_object.dir/print_object.c.o: /home/abdullah/Desktop/BSCS/Semester\ -5/OS/Project/cJson/tests/print_object.c
tests/CMakeFiles/print_object.dir/print_object.c.o: tests/CMakeFiles/print_object.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object tests/CMakeFiles/print_object.dir/print_object.c.o"
	cd "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/build/tests" && /usr/bin/c89-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT tests/CMakeFiles/print_object.dir/print_object.c.o -MF CMakeFiles/print_object.dir/print_object.c.o.d -o CMakeFiles/print_object.dir/print_object.c.o -c "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/cJson/tests/print_object.c"

tests/CMakeFiles/print_object.dir/print_object.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/print_object.dir/print_object.c.i"
	cd "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/build/tests" && /usr/bin/c89-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/cJson/tests/print_object.c" > CMakeFiles/print_object.dir/print_object.c.i

tests/CMakeFiles/print_object.dir/print_object.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/print_object.dir/print_object.c.s"
	cd "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/build/tests" && /usr/bin/c89-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/cJson/tests/print_object.c" -o CMakeFiles/print_object.dir/print_object.c.s

# Object files for target print_object
print_object_OBJECTS = \
"CMakeFiles/print_object.dir/print_object.c.o"

# External object files for target print_object
print_object_EXTERNAL_OBJECTS =

tests/print_object: tests/CMakeFiles/print_object.dir/print_object.c.o
tests/print_object: tests/CMakeFiles/print_object.dir/build.make
tests/print_object: libcjson.so.1.7.18
tests/print_object: tests/libunity.a
tests/print_object: tests/CMakeFiles/print_object.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable print_object"
	cd "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/build/tests" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/print_object.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/print_object.dir/build: tests/print_object
.PHONY : tests/CMakeFiles/print_object.dir/build

tests/CMakeFiles/print_object.dir/clean:
	cd "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/build/tests" && $(CMAKE_COMMAND) -P CMakeFiles/print_object.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/print_object.dir/clean

tests/CMakeFiles/print_object.dir/depend:
	cd "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/cJson" "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/cJson/tests" "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/build" "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/build/tests" "/home/abdullah/Desktop/BSCS/Semester -5/OS/Project/build/tests/CMakeFiles/print_object.dir/DependInfo.cmake" "--color=$(COLOR)"
.PHONY : tests/CMakeFiles/print_object.dir/depend

