cmake_minimum_required(VERSION 3.2)

project(offshore VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB OffshoreSources CONFIGURE_DEPENDS src/**/*.cpp src/**/*.hpp src/**/*.c src/**/*.h)

add_executable(offshore ${OffshoreSources})

target_compile_definitions(offshore PRIVATE GLM_FORCE_ALIGNED GLM_FORCE_SSE)

target_include_directories(offshore SYSTEM PRIVATE thirdparty)
target_include_directories(offshore SYSTEM PRIVATE assets/shader/include)
# target_link_options(offshore PRIVATE -fsanitize=address -fno-omit-frame-pointer)

target_compile_definitions(
  offshore
  PRIVATE
  NK_INCLUDE_FIXED_TYPES
  NK_INCLUDE_STANDARD_IO
  NK_INCLUDE_DEFAULT_ALLOCATOR
  NK_INCLUDE_VERTEX_BUFFER_OUTPUT
  NK_INCLUDE_FONT_BAKING
  NK_INCLUDE_DEFAULT_FONT
  NK_INCLUDE_STANDARD_VARARGS
  SOKOL_NUKLEAR_NO_SOKOL_APP
)

if(MSVC)
  target_compile_definitions(offshore PRIVATE _HAS_EXCEPTIONS=0)

  target_compile_options(offshore PRIVATE /W4 /WX /GR-)
else()
  target_compile_definitions(offshore PRIVATE _GLIBCXX_DEBUG _LIBCPP_DEBUG)

  target_compile_options(offshore PRIVATE -Wall -Wextra -Wpedantic -fno-exceptions -fno-rtti)
  target_compile_options(offshore PRIVATE -O0 -g)

  target_link_options(offshore PRIVATE -lSDL2 -lGL)
endif()

