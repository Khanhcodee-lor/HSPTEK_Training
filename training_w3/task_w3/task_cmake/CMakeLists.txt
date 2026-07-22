cmake_minimum_required(VERSION 3.16)
project(AdvancedGeometryApp LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Quét file nguồn tự động
file(GLOB_RECURSE GEOMETRY_SRCS "src/geometry/*.cpp")
file(GLOB_RECURSE UTILS_SRCS "src/utils/*.cpp")

add_library(geometry_lib STATIC ${GEOMETRY_SRCS})
add_library(utils_lib STATIC ${UTILS_SRCS})

add_executable(geo_app src/main.cpp)

target_link_libraries(geo_app PRIVATE geometry_lib utils_lib)