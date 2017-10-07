cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(TetrixWine)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-g -pthread)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

add_executable(start main.cc)
target_link_libraries(start ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})

