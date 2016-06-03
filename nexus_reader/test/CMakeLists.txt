enable_testing()
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

set(tests_LINK_LIBRARIES ${GTEST_LIBRARIES}
                         eventdata_lib
                         testHelpers_lib)
if(UNIX)
    list(APPEND tests_LINK_LIBRARIES pthread)
endif()

add_executable(eventDataTest eventDataTest.cpp unitTestRunner.cpp)
target_link_libraries(eventDataTest ${tests_LINK_LIBRARIES})
