# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/fanzhuzhi/Desktop/PFE/fundvid-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/fanzhuzhi/Desktop/PFE/fundvid-master/build_dir

# Include any dependencies generated for this target.
include CMakeFiles/density_map_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/density_map_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/density_map_test.dir/flags.make

CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o: CMakeFiles/density_map_test.dir/flags.make
CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o: ../src/test/density_map_test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fanzhuzhi/Desktop/PFE/fundvid-master/build_dir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o -c /home/fanzhuzhi/Desktop/PFE/fundvid-master/src/test/density_map_test.cpp

CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fanzhuzhi/Desktop/PFE/fundvid-master/src/test/density_map_test.cpp > CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.i

CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fanzhuzhi/Desktop/PFE/fundvid-master/src/test/density_map_test.cpp -o CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.s

CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o.requires:

.PHONY : CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o.requires

CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o.provides: CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o.requires
	$(MAKE) -f CMakeFiles/density_map_test.dir/build.make CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o.provides.build
.PHONY : CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o.provides

CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o.provides.build: CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o


# Object files for target density_map_test
density_map_test_OBJECTS = \
"CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o"

# External object files for target density_map_test
density_map_test_EXTERNAL_OBJECTS =

bin/density_map_test: CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o
bin/density_map_test: CMakeFiles/density_map_test.dir/build.make
bin/density_map_test: libmyMisc.so
bin/density_map_test: src/third_party/orsa-fundamental_1.0/src/libOrsa/libOrsa.a
bin/density_map_test: /usr/local/lib/libopencv_videostab.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_ts.a
bin/density_map_test: /usr/local/lib/libopencv_superres.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_stitching.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_contrib.so.2.4.13
bin/density_map_test: libmyIO.so
bin/density_map_test: libmyMatch.so
bin/density_map_test: src/third_party/orsa-fundamental_1.0/src/extras/libNumerics/libNumerics.a
bin/density_map_test: /usr/local/lib/libopencv_nonfree.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_ocl.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_gpu.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_photo.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_objdetect.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_legacy.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_video.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_ml.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_calib3d.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_features2d.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_highgui.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_imgproc.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_flann.so.2.4.13
bin/density_map_test: /usr/local/lib/libopencv_core.so.2.4.13
bin/density_map_test: CMakeFiles/density_map_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fanzhuzhi/Desktop/PFE/fundvid-master/build_dir/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/density_map_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/density_map_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/density_map_test.dir/build: bin/density_map_test

.PHONY : CMakeFiles/density_map_test.dir/build

CMakeFiles/density_map_test.dir/requires: CMakeFiles/density_map_test.dir/src/test/density_map_test.cpp.o.requires

.PHONY : CMakeFiles/density_map_test.dir/requires

CMakeFiles/density_map_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/density_map_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/density_map_test.dir/clean

CMakeFiles/density_map_test.dir/depend:
	cd /home/fanzhuzhi/Desktop/PFE/fundvid-master/build_dir && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fanzhuzhi/Desktop/PFE/fundvid-master /home/fanzhuzhi/Desktop/PFE/fundvid-master /home/fanzhuzhi/Desktop/PFE/fundvid-master/build_dir /home/fanzhuzhi/Desktop/PFE/fundvid-master/build_dir /home/fanzhuzhi/Desktop/PFE/fundvid-master/build_dir/CMakeFiles/density_map_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/density_map_test.dir/depend

