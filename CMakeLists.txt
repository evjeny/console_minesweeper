cmake_minimum_required(VERSION 3.20)
project(console_minesweeper)

set(CMAKE_CXX_STANDARD 17)

add_executable(console_minesweeper src/main.cpp src/field.cpp include/field.h include/color_print.h)