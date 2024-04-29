cmake_minimum_required(VERSION 3.10)
project(glad)

set(CMAKE_C_STANDARD 11)
add_compile_options(-g)

file(GLOB SOURCES src/*.c)

add_library(glad STATIC ${SOURCES})
include_directories(${PROJECT_SOURCE_DIR}/include)
