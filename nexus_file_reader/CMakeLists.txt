project(isis_nexus_streamer)

set( SRC_FILES
        src/NexusFileReader.cpp)

set( INC_FILES
        include/EventPublisher.h
        include/NexusFileReader.h)

set( TEST_FILES
        test/NexusFileReaderTest.cpp)

#####################
## Libraries       ##
#####################

include_directories(
        ${HDF5_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include)

add_library(nexusFileReader_lib ${PROJECT_SOURCE_DIR}/src/NexusFileReader.cpp)

target_link_libraries(nexusFileReader_lib
        ${HDF5_LIBRARIES}
        ${HDF5_CXX_LIBRARIES})

#####################
## Unit Tests      ##
#####################

set(tests_LINK_LIBRARIES
        gtest
        gmock
        nexusFileReader_lib)
if(UNIX)
    list(APPEND tests_LINK_LIBRARIES pthread)
endif()

add_library(fileReaderUnitTests
        ${TEST_FILES})
target_link_libraries(fileReaderUnitTests ${tests_LINK_LIBRARIES})
