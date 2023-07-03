cmake_minimum_required(VERSION 3.16)

project(offshore VERSION 0.1 LANGUAGES CXX C)

# GLOBAL

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT WIN32)
  set(USE_SHARED_THIRDPARTY_LIBS ON)
endif()

set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDebug)

set(CMAKE_CXX_FLAGS_DISTRIBUTION "${CMAKE_CXX_FLAGS_RELEASE}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DISTRIBUTION TRUE)

# JoltPhysics

if(USE_SHARED_THIRDPARTY_LIBS)
  set(BUILD_SHARED_LIBS ON CACHE BOOL "Compile Jolt as a shared library")
endif()

add_subdirectory(thirdparty/JoltPhysics/Build)

# NVRHI

if(USE_SHARED_THIRDPARTY_LIBS)
  set(NVRHI_BUILD_SHARED ON CACHE BOOL "Build NVRHI as a shared library (DLL or .so)")
endif()

set(NVRHI_WITH_SHADER_COMPILER OFF CACHE BOOL "Build the NVRHI shader compiler executable")
add_subdirectory(thirdparty/nvrhi)

set(SHADERMAKE_BIN_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/tools/bin" CACHE STRING "")
add_subdirectory(tools/ShaderMake)

# OFFSHORE

file(GLOB OffshoreSources CONFIGURE_DEPENDS src/**/*.cpp src/**/*.hpp src/**/*.c src/**/*.h)
file(GLOB ThirdpartySources CONFIGURE_DEPENDS thirdparty/vk-bootstrap/*.cpp thirdparty/vk-bootstrap/*.h)

add_executable(offshore ${OffshoreSources} ${ThirdpartySources})

if(NOT USE_SHARED_THIRDPARTY_LIBS)
  target_link_libraries(offshore PRIVATE nvrhi_vk)
endif()

target_link_libraries(offshore PRIVATE nvrhi Jolt)

target_include_directories(offshore SYSTEM PRIVATE thirdparty)
target_include_directories(offshore SYSTEM PRIVATE assets/shader/include)
target_include_directories(offshore SYSTEM PRIVATE thirdparty/nvrhi/include)

add_compile_definitions(
  GLM_FORCE_ALIGNED
  GLM_FORCE_SSE2
  NK_INCLUDE_FIXED_TYPES
  NK_INCLUDE_STANDARD_IO
  NK_INCLUDE_DEFAULT_ALLOCATOR
  NK_INCLUDE_VERTEX_BUFFER_OUTPUT
  NK_INCLUDE_FONT_BAKING
  NK_INCLUDE_DEFAULT_FONT
  NK_INCLUDE_STANDARD_VARARGS
  JPH_DOUBLE_PRECISION
)

# PLATFORM

if(MSVC)
  configure_file(platform/win32/lib/SDL2.dll ${CMAKE_BINARY_DIR} COPYONLY)

  target_include_directories(offshore PRIVATE platform/win32/include)
  target_link_directories(offshore PRIVATE platform/win32/lib)
  target_link_libraries(offshore PRIVATE SDL2)

  target_compile_options(offshore PRIVATE /W4)
else()
  target_compile_definitions(offshore PRIVATE $<$<CONFIG:Debug>:_GLIBCXX_DEBUG _LIBCPP_DEBUG>)
  target_compile_options(offshore PRIVATE -Wall -Wextra -Wpedantic)

  target_link_libraries(offshore PRIVATE SDL2)

  target_link_options(offshore PRIVATE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer>)
  # target_link_options(offshore PRIVATE $<$<CONFIG:Debug>:-fsanitize=undefined,implicit-conversion,float-divide-by-zero,local-bounds -fno-omit-frame-pointer>)
  # target_link_libraries(offshore PRIVATE efence)
endif()

# VERBOSE

message("-- CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
message("-- CMAKE_CXX_FLAGS_DEBUG: " ${CMAKE_CXX_FLAGS_DEBUG})
message("-- CMAKE_CXX_FLAGS_RELEASE: " ${CMAKE_CXX_FLAGS_RELEASE})
message("-- CMAKE_CXX_FLAGS_DISTRIBUTION: " ${CMAKE_CXX_FLAGS_DISTRIBUTION})
