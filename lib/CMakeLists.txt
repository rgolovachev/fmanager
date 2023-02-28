cmake_minimum_required(VERSION 3.22)

project(shared)

set(SRC_LIST exec.c)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "..")
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
