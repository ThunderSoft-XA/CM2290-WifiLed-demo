cmake_minimum_required(VERSION 3.10)
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_FLAGS " -std=gnu++11 -O3")
project("mqtt-led-client"
                VERSION "1.0.1"
                LANGUAGES CXX
        )

find_package(Threads REQUIRED)

## include directories
include_directories( ${CMAKE_CURRENT_SOURCE_DIR})

## binary files
add_executable( ${PROJECT_NAME}
                ./mqtt_control.cpp
                ./led_test.cpp
        )

## link binaries
target_link_libraries( ${PROJECT_NAME}
                -lmosquitto
        )

set(INSTALL_TARGETS
                mqtt-led-client
        )

## install binaries
install(TARGETS ${INSTALL_TARGETS}
                RUNTIME DESTINATION bin
                LIBRARY DESTINATION lib
        )