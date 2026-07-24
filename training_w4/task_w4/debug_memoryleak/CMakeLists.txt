cmake_minimum_required(VERSION 3.16)
project(DeviceManagerApp C)

# Cấu hình chuẩn C11 và cờ Debug để dùng với Valgrind
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_BUILD_TYPE Debug)

# Khai báo đường dẫn thư mục chứa Header (.h)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 1. Gom nhóm mã nguồn và Đóng gói thành Thư viện tĩnh (Static Library)
file(GLOB LIB_SOURCES "src/*.c")
add_library(device_core STATIC ${LIB_SOURCES})

# 2. Tạo file thực thi từ app/main.c
add_executable(device_app app/main.c)

# 3. Liên kết file thực thi với thư viện tĩnh device_core vừa tạo
target_link_libraries(device_app PRIVATE device_core)