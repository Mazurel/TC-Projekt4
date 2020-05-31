cmake_minimum_required(VERSION 3.17)
project(Pojekt4)

set(CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_executable(Projekt4 main.cpp)
target_link_libraries(Projekt4 sfml-graphics sfml-system sfml-window)