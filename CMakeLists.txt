cmake_minimum_required(VERSION 3.12)

project(ecs)

add_subdirectory(raylib)
add_executable(game game.cpp ecs.cpp)
add_executable(test test.cpp ecs.cpp)

set_property(TARGET game PROPERTY CXX_STANDARD 20)
set_property(TARGET test PROPERTY CXX_STANDARD 20)

target_link_libraries(game raylib)


