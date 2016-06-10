project(isis_nexus_streamer)

find_package(HDF5 COMPONENTS CXX HL REQUIRED)

#####################
## Libraries       ##
#####################

include_directories(
        ${HDF5_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include)

add_library(eventdata_lib ${PROJECT_SOURCE_DIR}/src/EventData.cpp)
add_library(nexusFileReader_lib ${PROJECT_SOURCE_DIR}/src/NexusFileReader.cpp)
add_library(testHelpers_lib ${PROJECT_SOURCE_DIR}/src/EventDataTestHelper.cpp)
add_library(nexusStreamer_lib ${PROJECT_SOURCE_DIR}/src/NexusStreamer.cpp)

target_link_libraries(nexusFileReader_lib
        ${HDF5_LIBRARIES}
        ${HDF5_CXX_LIBRARIES})

#####################
## Main executable ##
#####################

add_executable(main_eventdata ${PROJECT_SOURCE_DIR}/src/main.cpp)

#####################
## Unit Tests      ##
#####################

include(CTest)
enable_testing()

set(tests_LINK_LIBRARIES
        gtest
        gmock
        eventdata_lib
        nexusFileReader_lib
        nexusStreamer_lib
        testHelpers_lib)
if(UNIX)
    list(APPEND tests_LINK_LIBRARIES pthread)
endif()

add_executable(unitTests
        test/EventDataTest.cpp
        test/NexusFileReaderTest.cpp
        test/NexusStreamerTest.cpp
        test/unitTestRunner.cpp)
target_link_libraries(unitTests ${tests_LINK_LIBRARIES})

# Make the tests available to run through ctest
# Do "ctest -VV" in the build directory
add_test(NAME all_tests COMMAND unitTests "${PROJECT_SOURCE_DIR}/test/data/")
