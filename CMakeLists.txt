cmake_minimum_required(VERSION 3.24)
project(tetris)

set(CMAKE_CXX_STANDARD 17)

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})

link_directories(${X11_LIBRARIES})


add_executable(${PROJECT_NAME} src/olcPixelGameEngine.hpp src/main.cpp )
target_include_directories(${PROJECT_NAME} PUBLIC "src")
target_link_libraries(${PROJECT_NAME} X11 GL png stdc++fs pthread)
