project(isis_nexus_streamer)

find_package(HDF5 COMPONENTS CXX HL REQUIRED)

#####################
## Libraries       ##
#####################

include_directories(
        ${HDF5_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/inc)

add_library(eventdata_lib ${PROJECT_SOURCE_DIR}/src/eventData.cpp)
add_library(nexusFileReader_lib ${PROJECT_SOURCE_DIR}/src/nexusFileReader.cpp)
add_library(testHelpers_lib ${PROJECT_SOURCE_DIR}/src/eventDataTestHelper.cpp)
#add_library(eventKafkaProducer_lib ${PROJECT_SOURCE_DIR}/src/eventKafkaProducer.cpp)

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

enable_testing()
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

set(tests_LINK_LIBRARIES
        ${GTEST_LIBRARIES}
        eventdata_lib
        nexusFileReader_lib
        testHelpers_lib)
if(UNIX)
    list(APPEND tests_LINK_LIBRARIES pthread)
endif()

add_executable(unitTests
        test/eventDataTest.cpp
        test/nexusFileReaderTest.cpp
        test/eventKafkaProducerTest.cpp
        test/unitTestRunner.cpp)
target_link_libraries(unitTests ${tests_LINK_LIBRARIES})
