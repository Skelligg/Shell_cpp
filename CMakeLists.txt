cmake_minimum_required(VERSION 3.13)

project(shell-starter-cpp)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard

add_executable(shell ${SOURCE_FILES}
        src/shell.cpp
        src/shell.h
        src/Autocompleter.cpp
        src/Autocompleter.h)

target_link_libraries(shell PRIVATE readline)
