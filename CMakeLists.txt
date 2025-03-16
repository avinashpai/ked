cmake_minimum_required(VERSION 3.20)
project(Ked)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -fcolor-diagnostics")

file(GLOB_RECURSE SRC_FILES src/*.cpp)
find_package(Curses REQUIRED)

include_directories(${CURSES_INCLUDE_DIR})

add_executable(Ked ${SRC_FILES})
target_link_libraries(Ked PRIVATE ${CURSES_LIBRARIES})
