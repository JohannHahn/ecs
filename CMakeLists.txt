cmake_minimum_required(VERSION 3.12)

project(ecs)

add_subdirectory(raylib)
add_executable(ecs_test main.cpp ecs.cpp)

set_property(TARGET ecs_test PROPERTY CXX_STANDARD 20)

target_link_libraries(ecs_test raylib)

