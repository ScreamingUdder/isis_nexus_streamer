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
        eventDataTest.cpp
        nexusFileReaderTest.cpp
        unitTestRunner.cpp)

target_link_libraries(unitTests ${tests_LINK_LIBRARIES})
