cmake_minimum_required(VERSION 3.2)

project(nvport VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(Vulkan REQUIRED)

add_subdirectory(nvrhi)

# add_executable(nvport main.cpp VkBootstrap.cpp VkBootstrap.h VkBootstrapDispatch.h)
add_executable(nvport main.cpp)
target_link_libraries(nvport PUBLIC nvrhi)

# target_include_directories(nvport PUBLIC ${Vulkan_INCLUDE_DIRS})
# target_link_directories(nvport PUBLIC ${Vulkan_LIBRARY_DIRS})
# target_compile_definitions(nvport PUBLIC ${Vulkan_DEFINITIONS})

target_link_options(nvport PUBLIC -lSDL2 -lvulkan)
