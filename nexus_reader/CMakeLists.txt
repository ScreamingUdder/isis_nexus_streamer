project(isis_nexus_streamer)

find_package(HDF5)

#####################
## Libraries       ##
#####################

include_directories(${PROJECT_SOURCE_DIR}/inc)

add_library(eventdata_lib ${PROJECT_SOURCE_DIR}/src/eventData.cpp)
add_library(testHelpers_lib ${PROJECT_SOURCE_DIR}/src/eventDataTestHelper.cpp)


#####################
## Main executable ##
#####################

add_executable(main_eventdata ${PROJECT_SOURCE_DIR}/src/main.cpp)
#target_link_libraries(eventdata_lib)
