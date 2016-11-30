cmake_minimum_required(VERSION 3.6)
project(MIDIUSB_1x1)

find_package(GTest REQUIRED)
include_directories(. ./test ${GTEST_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES test/test_midi_bridge.cpp )
add_executable(MIDIUSB_1x1 ${SOURCE_FILES})
target_link_libraries(MIDIUSB_1x1 ${GTEST_LIBRARIES})
