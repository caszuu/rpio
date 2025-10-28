cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# select the target board type
set(PICO_BOARD pico2 CACHE STRING "Board type")

# define firmware option configuration

# setup pico-sdk and rpio project
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "2.0.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 2.0.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(rpio C CXX ASM)
pico_sdk_init()

add_subdirectory(src)
