project(isis_nexus_streamer)

set( SRC_FILES
        src/EventData.cpp)

set( INC_FILES
        include/EventData.h)

set( TEST_FILES
        test/EventDataTest.cpp)

#####################
## Libraries       ##
#####################

include_directories(
        ${PROJECT_SOURCE_DIR}/include)

add_library(eventdata_lib ${PROJECT_SOURCE_DIR}/src/EventData.cpp)
add_library(eventDataTestHelper_lib ${PROJECT_SOURCE_DIR}/src/EventDataTestHelper.cpp)

#####################
## Unit Tests      ##
#####################

set(eventData_tests_LINK_LIBRARIES
        gtest
        gmock
        eventdata_lib
        eventDataTestHelper_lib)
if(UNIX)
    list(APPEND tests_LINK_LIBRARIES pthread)
endif()

add_executable(eventDataUnitTests
        test/EventDataTest.cpp
        test/eventDataTestRunner.cpp)
target_link_libraries(eventDataUnitTests ${eventData_tests_LINK_LIBRARIES})

# Make the tests available to run through ctest
# Do "ctest -VV" in the build directory
add_test(NAME eventData_tests COMMAND eventDataUnitTests)

#####################
## Test Coverage   ##
#####################

# Add all src and inc files to test coverage check
if (COVERALLS)
    foreach( loop_var ${SRC_FILES} ${INC_FILES})
        set_property(GLOBAL APPEND PROPERTY COVERAGE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}")
    endforeach(loop_var)
endif()
