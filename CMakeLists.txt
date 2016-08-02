cmake_minimum_required(VERSION 2.8)
project(isis_nexus_streamer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(HDF5 COMPONENTS CXX HL REQUIRED)
find_package(LibRDKafka)
find_package(Tcmalloc)

include(CTest)
enable_testing()

# We probably don't want this to run on every build.
option(COVERALLS "Generate coveralls data" OFF)

if (COVERALLS)
    include(Coveralls)
    coveralls_turn_on_coverage()
endif()

#####################
## TCMalloc        ##
#####################

# if tcmalloc is available then link all executables against it
if (TCMALLOC_FOUND)
    macro (add_executable _name)
        # invoke built-in add_executable
        _add_executable(${ARGV})
        if (TARGET ${_name})
            target_link_libraries(${_name} tcmalloc_minimal)
        endif()
    endmacro()
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

# download and  build gtest and gmock
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
add_subdirectory("${PROJECT_SOURCE_DIR}/nexus_file_reader")

####################
## Unit tests     ##
####################

include_directories(
        ${HDF5_INCLUDE_DIRS}
        ${LibRDKafka_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/nexus_consumer/include
        ${PROJECT_SOURCE_DIR}/nexus_producer/include
        ${PROJECT_SOURCE_DIR}/event_data/include
        ${PROJECT_SOURCE_DIR}/nexus_file_reader/include)

file(GLOB TEST_SRC_FILES
        ${PROJECT_SOURCE_DIR}/nexus_consumer/test/*.cpp
        ${PROJECT_SOURCE_DIR}/nexus_producer/test/*.cpp
        ${PROJECT_SOURCE_DIR}/event_data/test/*.cpp
        ${PROJECT_SOURCE_DIR}/nexus_file_reader/test/*.cpp
        unitTestRunner.cpp)

add_executable(unitTests ${TEST_SRC_FILES})

target_link_libraries(unitTests
        gtest
        gmock
        producerUnitTests
        consumerUnitTests
        eventDataUnitTests
        fileReaderUnitTests)

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
# Download jmxterm and kafka binaries to use for collecting metrics
# if the file already exists with the correct checksum then it won't be redownloaded
file(DOWNLOAD http://downloads.sourceforge.net/cyclops-group/jmxterm-1.0-alpha-4-uber.jar
        ${CMAKE_CURRENT_BINARY_DIR}/system_tests/jmxterm.jar
        TIMEOUT 30
        EXPECTED_MD5 f3c307163673d9f70c27b3f440df9dd4)
file(DOWNLOAD http://mirror.ox.ac.uk/sites/rsync.apache.org/kafka/0.9.0.1/kafka_2.11-0.9.0.1.tgz
        ${CMAKE_CURRENT_BINARY_DIR}/system_tests/kafka_2.11-0.9.0.1.tgz
        TIMEOUT 30
        EXPECTED_MD5 b71e5cbc78165c1ca483279c27402663)
execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_CURRENT_BINARY_DIR}/system_tests/kafka_2.11-0.9.0.1.tgz
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/system_tests
)

find_package(PythonInterp REQUIRED)
set(system_test_args )
add_custom_target(system_test COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/system_tests/test.py ${PROJECT_SOURCE_DIR}/data -p -j 9997
        COMMENT "Executing system tests..." VERBATIM)
add_custom_target(system_test_virtual COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/system_tests/test.py ${PROJECT_SOURCE_DIR}/data
        COMMENT "Executing system tests..." VERBATIM)
