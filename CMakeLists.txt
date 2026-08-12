cmake_minimum_required(VERSION 3.20)
project(httpget C CXX)

set(CMAKE_C_COMPILER /opt/devkitpro/devkitARM/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER /opt/devkitpro/devkitARM/bin/arm-none-eabi-g++)

set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

add_definitions(-DARM9 -D__NDS__)

include_directories(/opt/devkitpro/libnds/include)
include_directories(/opt/devkitpro/devkitARM/arm-none-eabi/include)
include_directories(/opt/devkitpro/calico/include)

file(GLOB_RECURSE SOURCES "source/*.c" "source/*.cpp" "*.c" "*.cpp")
add_executable(httpget ${SOURCES}
        source/connection.h
        source/connection.c
        source/saveMenu.h
        source/saveMenu.c)
