cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(TetrixWine)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-g -pthread)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

include_directories(include)

add_executable(start src/main.cc)
add_executable(database src/database.cxx)

target_link_libraries(start ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(database ${CMAKE_THREAD_LIBS_INIT})

