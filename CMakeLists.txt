cmake_minimum_required(VERSION 3.10)
project(gfx)

message("Compiler: ${CMAKE_C_COMPILER_ID}")
message("Compiler: ${CMAKE_C_COMPILER}")
message("Workspace: ${PROJECT_SOURCE_DIR}")
set(CMAKE_C_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_compile_options(
  -g -Wall -Wextra -Wpedantic -Wstrict-aliasing -fsanitize=address
  -ffile-prefix-map=${PROJECT_SOURCE_DIR}/src=
  -Wno-gnu-zero-variadic-macro-arguments -Wno-pointer-arith
)
add_link_options(-fsanitize=address)

file(GLOB SOURCES src/*.c src/**/*.c)

add_compile_definitions(GLFW_INCLUDE_NONE)
add_executable(gfx ${SOURCES})

include_directories($ENV{GLFW_INCLUDE_DIR} $ENV{GLAD_INCLUDE_DIR})
link_directories($ENV{GLFW_LIB_DIR} $ENV{GLAD_LIB_DIR})

target_link_libraries(gfx glfw3 glad m)
