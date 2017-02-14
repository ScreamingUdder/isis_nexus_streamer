project(isis_nexus_streamer)

find_package(LibRDKafka)

set( SRC_FILES
        src/NexusSubscriber.cpp
        src/NexusFileWriter.cpp)

set( INC_FILES
        include/NexusSubscriber.h
        include/NexusFileWriter.h)

set( TEST_FILES
        test/NexusSubscriberTest.cpp
        test/NexusFileWriterTest.cpp)

#####################
## Libraries       ##
#####################

include_directories(
        ${LibRDKafka_INCLUDE_DIR}
        ${HDF5_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/../event_data/include)

add_library(nexusSubscriber_lib ${PROJECT_SOURCE_DIR}/src/NexusSubscriber.cpp)
add_library(eventSubscriber_lib
        ${PROJECT_SOURCE_DIR}/src/KafkaEventSubscriber.cpp)
add_library(nexusFileWriter_lib ${PROJECT_SOURCE_DIR}/src/NexusFileWriter.cpp)

target_link_libraries(eventSubscriber_lib
        ${LibRDKafka_LIBRARIES})

target_link_libraries(nexusSubscriber_lib
        eventdata_lib)

target_link_libraries(nexusFileWriter_lib
        eventdata_lib
        ${HDF5_LIBRARIES}
        ${HDF5_CXX_LIBRARIES})

set(core_libraries
        eventSubscriber_lib
        nexusSubscriber_lib
        nexusFileWriter_lib
        nexusFileReader_lib)

#####################
## Main executable ##
#####################

add_executable(main_nexusSubscriber ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(main_nexusSubscriber
        ${core_libraries})

install(TARGETS main_nexusSubscriber DESTINATION bin)

#####################
## Unit Tests      ##
#####################

set(tests_LINK_LIBRARIES
        gtest
        gmock
        ${core_libraries})
if(UNIX)
    list(APPEND tests_LINK_LIBRARIES pthread)
endif()

add_library(consumerUnitTests
        ${TEST_FILES})
target_link_libraries(consumerUnitTests ${tests_LINK_LIBRARIES})

#####################
## Test Coverage   ##
#####################

# Add all src and inc files to test coverage check
if (COVERALLS)
    foreach( loop_var ${SRC_FILES} ${INC_FILES})
        set_property(GLOBAL APPEND PROPERTY COVERAGE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}")
    endforeach(loop_var)
endif()

#####################
## clang-format    ##
#####################

add_check_clang_format(check_format_nexus_consumer)
