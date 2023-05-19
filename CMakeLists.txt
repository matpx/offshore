cmake_minimum_required(VERSION 3.2)

project(offshore VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB OffshoreSources CONFIGURE_DEPENDS src/*.cpp src/*.hpp src/**/*.cpp src/**/*.hpp)

add_executable(offshore ${OffshoreSources})

target_compile_definitions(offshore PRIVATE GLM_FORCE_ALIGNED GLM_FORCE_SSE)

target_include_directories(offshore SYSTEM PRIVATE thirdparty)
target_include_directories(offshore SYSTEM PRIVATE assets/shader/include)
target_link_options(offshore PRIVATE -lSDL2 -lGL)
# target_link_options(offshore PRIVATE -fsanitize=address -fno-omit-frame-pointer)

if(MSVC)
  target_compile_options(offshore PRIVATE /W4 /WX)
else()
  target_compile_options(offshore PRIVATE -Wall -Wextra -Wpedantic -fno-exceptions -fno-rtti)
  target_compile_options(offshore PRIVATE -O0 -g)
endif()

