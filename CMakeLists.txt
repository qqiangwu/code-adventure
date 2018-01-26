cmake_minimum_required(VERSION 3.2)
project(expr)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED)

add_subdirectory(logging)
add_subdirectory(net)
