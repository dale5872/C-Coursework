cmake_minimum_required(VERSION 3.13)
project(task_01)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(f29oc_lab1 stacktest.c
stack.h
stack.c)
