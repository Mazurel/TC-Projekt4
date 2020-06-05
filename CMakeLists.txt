cmake_minimum_required(VERSION 3.17)
project(Pojekt4)

if (WIN32)
    set(SFML_LOKALIZACJA "C:\\Sfml-2.5.1")
    find_package(SFML COMPONENTS graphics window system HINTS ${SFML_LOKALIZACJA})
else()
    find_package(SFML COMPONENTS graphics window system)
endif()

set(CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

configure_file(${CMAKE_SOURCE_DIR}/dzwig.png ${CMAKE_BINARY_DIR}/dzwig.png COPYONLY)

add_executable(Projekt4 main.cpp)
target_link_libraries(Projekt4 sfml-graphics sfml-system sfml-window)
