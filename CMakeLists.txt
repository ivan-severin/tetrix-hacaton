cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(TetrixWine)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-g -pthread)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

find_package(Boost REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(include)

add_executable(start src/main.cc)
# add_executable(database src/database.cxx)
add_executable(face src/FaceRecognition.cc)

target_link_libraries(start ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})
# target_link_libraries(database ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(face ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} boost_system)

