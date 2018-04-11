cmake_minimum_required(VERSION 3.7)
project(SuperString)

set(CMAKE_CXX_STANDARD 11)

# "include" directories
include_directories(include)

# the SuperString library
add_library(SuperString STATIC src/SuperString.cc)
