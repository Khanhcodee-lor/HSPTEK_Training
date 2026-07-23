cmake_minimum_required(VERSION 3.16)
project(HelloArmApp LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(led_drive STATIC src/led_controller.cpp)

add_executable(${PROJECT_NAME} src/main.cpp src/led_controller.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
