# Install script for directory: /home/fanzhuzhi/Desktop/PFE/fundvid-master/src/third_party/orsa-fundamental_1.0/src/extras/libNumerics

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/src/extras/libNumerics" TYPE FILE FILES
    "/home/fanzhuzhi/Desktop/PFE/fundvid-master/src/third_party/orsa-fundamental_1.0/src/extras/libNumerics/CMakeLists.txt"
    "/home/fanzhuzhi/Desktop/PFE/fundvid-master/src/third_party/orsa-fundamental_1.0/src/extras/libNumerics/ccmath_svd.cpp"
    "/home/fanzhuzhi/Desktop/PFE/fundvid-master/src/third_party/orsa-fundamental_1.0/src/extras/libNumerics/matrix.cpp"
    "/home/fanzhuzhi/Desktop/PFE/fundvid-master/src/third_party/orsa-fundamental_1.0/src/extras/libNumerics/matrix.h"
    "/home/fanzhuzhi/Desktop/PFE/fundvid-master/src/third_party/orsa-fundamental_1.0/src/extras/libNumerics/numerics.cpp"
    "/home/fanzhuzhi/Desktop/PFE/fundvid-master/src/third_party/orsa-fundamental_1.0/src/extras/libNumerics/numerics.h"
    "/home/fanzhuzhi/Desktop/PFE/fundvid-master/src/third_party/orsa-fundamental_1.0/src/extras/libNumerics/vector.cpp"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/src/extras/libNumerics" TYPE FILE FILES "/home/fanzhuzhi/Desktop/PFE/fundvid-master/src/third_party/orsa-fundamental_1.0/src/extras/libNumerics/ccmath_svd_test.cpp")
endif()

