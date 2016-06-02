project(isis_nexus_streamer)

## eventdata library ##

include_directories(${PROJECT_SOURCE_DIR}/inc)
set(SRC_FILES
        ${PROJECT_SOURCE_DIR}/src/eventdata.cpp)
add_library(eventdata_lib ${SRC_FILES})

## Main executable ##

add_executable(main_eventdata ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(eventdata_lib)
