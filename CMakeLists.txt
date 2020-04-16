cmake_minimum_required(VERSION 3.15)
project(rayTracing)

set(CMAKE_CXX_STANDARD 20)

add_executable(rayTracing main.cpp myTypes.cpp myTypes.h)
target_link_libraries(rayTracing -lsfml-graphics -lsfml-window -lsfml-system)