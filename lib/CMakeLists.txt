cmake_minimum_required(VERSION 3.22)

project(shared)

### тут можно выбрать любой файл (например another.c)
set(SRC_LIST exec.c)
###
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
