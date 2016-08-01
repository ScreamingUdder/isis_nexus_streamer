project(isis_nexus_streamer)

if (BUILD_SHARED_LIBS AND WIN32)
    add_definitions(-DH5_BUILT_AS_DYNAMIC_LIB)
endif (BUILD_SHARED_LIBS AND WIN32)

set( SRC_FILES
        src/NexusFileReader.cpp
        src/NexusPublisher.cpp)

set( INC_FILES
        include/EventPublisher.h
        include/NexusFileReader.h
        include/NexusPublisher.h)

set( TEST_FILES
        test/NexusFileReaderTest.cpp
        test/NexusPublisherTest.cpp)

#####################
## Libraries       ##
#####################

include_directories(
        ${HDF5_INCLUDE_DIRS}
        ${LibRDKafka_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/../compatibility/include
        ${PROJECT_SOURCE_DIR}/include)

add_library(nexusFileReader_lib ${PROJECT_SOURCE_DIR}/src/NexusFileReader.cpp)
add_library(nexusPublisher_lib ${PROJECT_SOURCE_DIR}/src/NexusPublisher.cpp)
add_library(eventPublisher_lib
        ${PROJECT_SOURCE_DIR}/src/KafkaEventPublisher.cpp)

target_link_libraries(nexusFileReader_lib
        ${HDF5_LIBRARIES}
        ${HDF5_CXX_LIBRARIES})
target_link_libraries(eventPublisher_lib
        ${LibRDKafka_LIBRARIES})
target_link_libraries(nexusPublisher_lib
        nexusFileReader_lib
        eventdata_lib)

set(core_libraries
        eventdata_lib
        nexusFileReader_lib
        nexusPublisher_lib
        eventPublisher_lib)

#####################
## Main executable ##
#####################

if (WIN32)
    add_executable(main_nexusPublisher ${PROJECT_SOURCE_DIR}/src/main.cpp ${PROJECT_SOURCE_DIR}/../compatibility/src/wingetopt.c)
else ()
    add_executable(main_nexusPublisher ${PROJECT_SOURCE_DIR}/src/main.cpp)
endif (WIN32)

target_link_libraries(main_nexusPublisher
        ${core_libraries})

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

add_library(producerUnitTests
        ${TEST_FILES})
target_link_libraries(producerUnitTests ${tests_LINK_LIBRARIES})

#####################
## Test Coverage   ##
#####################

# Add all src and inc files to test coverage check
if (COVERALLS)
    foreach( loop_var ${SRC_FILES} ${INC_FILES})
        set_property(GLOBAL APPEND PROPERTY COVERAGE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}")
    endforeach(loop_var)
endif()
