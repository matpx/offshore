cmake_minimum_required(VERSION 3.2)

project(offshore VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB OffshoreSources src/*.cpp src/*.hpp)

add_executable(offshore ${OffshoreSources})

target_link_options(offshore PRIVATE -lSDL2)

