cmake_minimum_required(VERSION 2.8)
project(isis_nexus_streamer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_subdirectory("${PROJECT_SOURCE_DIR}/nexus_reader/test")
add_subdirectory("${PROJECT_SOURCE_DIR}/nexus_reader")
