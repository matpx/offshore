cmake_minimum_required(VERSION 3.2)

project(offshore VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB OffshoreSources CONFIGURE_DEPENDS src/**/*.cpp src/**/*.hpp src/**/*.c src/**/*.h)

file(GLOB ThirdpartySources CONFIGURE_DEPENDS thirdparty/vk-bootstrap/*.cpp thirdparty/vk-bootstrap/*.h)

set(OFFSHORE_BIN_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

add_executable(offshore ${OffshoreSources} ${ThirdpartySources})

set_target_properties (offshore PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OFFSHORE_BIN_OUTPUT_PATH}/$<CONFIG>")

target_compile_definitions(offshore PRIVATE GLM_FORCE_ALIGNED GLM_FORCE_SSE2)

target_include_directories(offshore SYSTEM PRIVATE thirdparty)
target_include_directories(offshore SYSTEM PRIVATE assets/shader/include)
target_link_options(offshore PRIVATE -fsanitize=address -fno-omit-frame-pointer)

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

set(NVRHI_WITH_SHADER_COMPILER OFF CACHE BOOL "Build the NVRHI shader compiler executable")
add_subdirectory(thirdparty/nvrhi)

set (SHADERMAKE_BIN_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/tools/bin" CACHE STRING "")
add_subdirectory(tools/ShaderMake)

target_include_directories(offshore SYSTEM PRIVATE thirdparty/nvrhi/include)
target_link_libraries(offshore PRIVATE nvrhi_vk nvrhi)

if(MSVC)
  target_compile_definitions(offshore PRIVATE _HAS_EXCEPTIONS=0)

  target_include_directories(offshore PRIVATE platform/win32/include)
  target_link_directories(offshore PRIVATE platform/win32/lib)
  target_link_libraries(offshore PRIVATE SDL2)

  target_compile_options(offshore PRIVATE /W4 /Od)

  configure_file(platform/win32/lib/SDL2.dll ${OFFSHORE_BIN_OUTPUT_PATH} COPYONLY)
else()
  target_compile_definitions(offshore PRIVATE _GLIBCXX_DEBUG _LIBCPP_DEBUG)

  target_compile_options(offshore PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(offshore PRIVATE -O0 -g)

  target_link_options(offshore PRIVATE -lSDL2)
endif()

