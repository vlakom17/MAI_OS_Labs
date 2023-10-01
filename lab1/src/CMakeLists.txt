cmake_minimum_required(VERSION 3.10)

project(OS_lab1)

set(CMAKE_C_STANDARD 11)

add_executable(child1 child1.c)

add_executable(child2 child2.c)

set(REQUIRED_FILES child1 child2)

add_executable(parent parent.c)