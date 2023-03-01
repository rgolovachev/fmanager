cmake_minimum_required(VERSION 3.22)

project(fmanager)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(SRC_LIST main.c perms.c fileops.c gui.c)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})