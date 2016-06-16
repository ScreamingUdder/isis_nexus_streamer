project(isis_nexus_streamer)

find_package(LibRDKafka)

set( SRC_FILES
        src/NexusSubscriber.cpp)

set( INC_FILES
        include/NexusSubscriber.h)

set( TEST_FILES
        test/NexusSubscriberTest.cpp)

#####################
## Libraries       ##
#####################

include_directories(
        ${LibRDKafka_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/include)

add_library(nexusSubscriber_lib ${PROJECT_SOURCE_DIR}/src/NexusSubscriber.cpp)
add_library(eventSubscriber_lib
        ${PROJECT_SOURCE_DIR}/src/KafkaEventSubscriber.cpp)

target_link_libraries(eventSubscriber_lib
        ${LibRDKafka_LIBRARIES})

set(core_libraries
        eventdata_lib
        nexusSubscriber_lib)

#####################
## Main executable ##
#####################

add_executable(main_nexusSubscriber ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(main_nexusSubscriber
        ${core_libraries})

#####################
## Unit Tests      ##
#####################

set(tests_LINK_LIBRARIES
        gtest
        gmock
        ${core_libraries})
        #testHelpers_lib)
if(UNIX)
    list(APPEND tests_LINK_LIBRARIES pthread)
endif()

add_executable(consumerUnitTests
        test/NexusSubscriberTest.cpp
        test/consumerUnitTestRunner.cpp)
target_link_libraries(consumerUnitTests ${tests_LINK_LIBRARIES})

# Make the tests available to run through ctest
# Do "ctest -VV" in the build directory
add_test(NAME consumer_tests COMMAND consumerUnitTests)

#####################
## Test Coverage   ##
#####################

# Add all src and inc files to test coverage check
if (COVERALLS)
    foreach( loop_var ${SRC_FILES} ${INC_FILES})
        set_property(GLOBAL APPEND PROPERTY COVERAGE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}")
    endforeach(loop_var)
endif()
