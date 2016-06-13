cmake_minimum_required(VERSION 2.8)
project(isis_nexus_streamer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# We probably don't want this to run on every build.
option(COVERALLS "Generate coveralls data" OFF)

if (COVERALLS)
    include(Coveralls)
    coveralls_turn_on_coverage()
endif()

#####################
## Get gtest       ##
#####################

if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject.cmake)
download_project(PROJ                googletest
        GIT_REPOSITORY      https://github.com/google/googletest.git
        GIT_TAG             master
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
        )

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

add_subdirectory("${PROJECT_SOURCE_DIR}/nexus_producer")

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
            "${PROJECT_SOURCE_DIR}/cmake"
    )
endif ()
