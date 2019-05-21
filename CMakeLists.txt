cmake_minimum_required(VERSION 3.12)
project(EveryLauncher VERSION 1.0.0)

option(USE_DXCB "integration with dxcb patfprm plugin" OFF)
option(E_DEBUG "turn on debug output" OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(E_DEBUG ON)
endif()

find_package(PkgConfig REQUIRED)
set(CMAKE_CXX_STANDARD 14)
add_subdirectory(src)
