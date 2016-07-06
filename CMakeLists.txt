cmake_minimum_required(VERSION 2.8)
project(isis_nexus_streamer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(HDF5 COMPONENTS CXX HL REQUIRED)
find_package(LibRDKafka)

include(CTest)
enable_testing()

# We probably don't want this to run on every build.
option(COVERALLS "Generate coveralls data" OFF)

if (COVERALLS)
    include(Coveralls)
    coveralls_turn_on_coverage()
endif()

#####################
## Doxygen         ##
#####################

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM
            )
endif(DOXYGEN_FOUND)

#####################
## Get gtest       ##
#####################

if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/DownloadProject.cmake)
download_project(PROJ                googletest
        GIT_REPOSITORY      https://github.com/google/googletest.git
        GIT_TAG             master
        ${UPDATE_DISCONNECTED_IF_AVAILABLE})

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

# When using CMake 2.8.11 or later, header path dependencies
# are automatically added to the gtest and gmock targets.
# For earlier CMake versions, we have to explicitly add the
# required directories to the header search path ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include"
            "${gmock_SOURCE_DIR}/include")
endif()

####################
## Add subdirs    ##
####################

add_subdirectory("${PROJECT_SOURCE_DIR}/event_data")
add_subdirectory("${PROJECT_SOURCE_DIR}/nexus_producer")
add_subdirectory("${PROJECT_SOURCE_DIR}/nexus_consumer")

####################
## Unit tests     ##
####################

include_directories(
        ${HDF5_INCLUDE_DIRS}
        ${LibRDKafka_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/nexus_consumer/include
        ${PROJECT_SOURCE_DIR}/nexus_producer/include
        ${PROJECT_SOURCE_DIR}/event_data/include)

file(GLOB TEST_SRC_FILES
        ${PROJECT_SOURCE_DIR}/nexus_consumer/test/*.cpp
        ${PROJECT_SOURCE_DIR}/nexus_producer/test/*.cpp
        ${PROJECT_SOURCE_DIR}/event_data/test/*.cpp
        unitTestRunner.cpp)

add_executable(unitTests ${TEST_SRC_FILES})

target_link_libraries(unitTests
        gtest
        gmock
        producerUnitTests
        consumerUnitTests
        eventDataUnitTests)

# Make the tests available to run through ctest
# Do "ctest -VV" in the build directory
add_test(NAME all_tests COMMAND unitTests "${PROJECT_SOURCE_DIR}/data/")

#####################
## Test coverage   ##
#####################

if (COVERALLS)
    get_property(ALL_SRCS GLOBAL PROPERTY COVERAGE_SRCS)
    set(SRCS_FILE "")
    foreach (SRC ${ALL_SRCS})
        set(SRCS_FILE "${SRCS_FILE}\n${SRC}")
    endforeach()
    #remove initial \n
    string(SUBSTRING ${SRCS_FILE} 1 -1 SRCS_FILE)
    set( SRCS_FILENAME "${CMAKE_CURRENT_BINARY_DIR}/sources.txt")
    file(WRITE ${SRCS_FILENAME} ${SRCS_FILE})
    coveralls_setup(
            "${SRCS_FILENAME}"
            ON
            "${PROJECT_SOURCE_DIR}/cmake")
endif ()

#####################
## System tests    ##
#####################

file(COPY ${PROJECT_SOURCE_DIR}/system_tests DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(DOWNLOAD http://downloads.sourceforge.net/cyclops-group/jmxterm-1.0-alpha-4-uber.jar
        ${CMAKE_CURRENT_BINARY_DIR}/system_tests/jmxterm.jar
        TIMEOUT 30)

find_package(PythonInterp REQUIRED)
add_custom_target(system_test COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/system_tests/test.py ${PROJECT_SOURCE_DIR}/data
        COMMENT "Executing system tests..." VERBATIM)
