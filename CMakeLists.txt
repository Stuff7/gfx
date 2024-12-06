cmake_minimum_required(VERSION 3.10)
project(gfx)

set(CMAKE_C_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
  -g -Wall -Wextra -Wpedantic -Wstrict-aliasing -fsanitize=address
  -ffile-prefix-map=${PROJECT_SOURCE_DIR}/src=
  -Wno-gnu-zero-variadic-macro-arguments -Wno-pointer-arith
)
add_link_options(-fsanitize=address)

file(GLOB SOURCES src/*.c src/**/*.c)

add_compile_definitions(GLFW_INCLUDE_NONE)

include_directories($ENV{GLFW_INCLUDE_DIR} $ENV{GLAD_INCLUDE_DIR})
link_directories($ENV{GLFW_LIB_DIR} $ENV{GLAD_LIB_DIR})

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

set(LIBRARIES glfw3 glad m)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${LIBRARIES})
