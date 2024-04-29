cmake_minimum_required(VERSION 3.10)
project(gfx)

message("Compiler: ${CMAKE_C_COMPILER_ID}")
message("Compiler: ${CMAKE_C_COMPILER}")
set(CMAKE_C_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_compile_options(-g -Wall -Wextra -Wpedantic -Wstrict-aliasing -fsanitize=address -Wno-gnu-zero-variadic-macro-arguments -Wno-pointer-arith)
add_link_options(-fsanitize=address)

file(GLOB SOURCES src/*.c)

add_compile_definitions(GLFW_INCLUDE_NONE)
add_executable(gfx ${SOURCES})

set(GLFW_DIR ${PROJECT_SOURCE_DIR}/lib/glfw)
set(GLAD_DIR ${PROJECT_SOURCE_DIR}/lib/glad)

include_directories(${GLFW_DIR}/include)
include_directories(${GLAD_DIR}/include)
target_link_libraries(gfx ${GLFW_DIR}/src/libglfw3.a)
target_link_libraries(gfx ${GLAD_DIR}/libglad.a)
target_link_libraries(gfx m)
