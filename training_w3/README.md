# Báo Cáo Thực Hành Tuần 3: Makefile, CMake & Cross-Compilation

Báo cáo chi tiết kết quả thực hành và học tập Tuần 3 về các kiến thức **Makefile**, **CMake (Build System)** và **Biên dịch chéo (Cross-Compilation cho ARM)**.

---

## 📑 Nội Dung Học Tập (Lý Thuyết)

Trong tuần 3, các kiến thức cốt lõi về hệ thống build system đã được tìm hiểu và áp dụng bao gồm:

### 1. Makefile Căn Bản & Nâng Cao
* **Cú pháp quy tắc (Rules)**: Khái niệm `target`, `prerequisite`, `recipe` và quy tắc thụt lề bằng phím **TAB**.
* **Ký tự viết tắt tự động (Automatic Variables)**:
  * `$@`: Đại diện cho tên **Target** hiện tại.
  * `$<`: Đại diện cho **Prerequisite đầu tiên**.
  * `$^`: Đại diện cho **Tất cả các Prerequisites**.
* **Quy tắc mẫu (Pattern Rules với `%`)**: Tự động hóa quy tắc biên dịch file `.cpp` thành file `.o` mà không cần viết tay từng file.
* **Auto Dependency (`.d` files)**: 
  * Sử dụng flag `-MMD -MP` để `g++` tự động quét danh sách các file header (`.hpp`) phụ thuộc.
  * Nạp file phụ thuộc vào Makefile bằng cú pháp `-include $(DEPS)` giúp tự động biên dịch lại khi sửa file `.hpp`.
* **Phony Targets (`.PHONY`)**: Khai báo các mục tiêu giả lập (`clean`, `run`, `all`) để tránh xung đột với tên file trên ổ cứng.
* **Quản lý cấu trúc thư mục `build/`**:
  * Tự động tạo thư mục `build/` chứa tất cả file trung gian `.o` và `.d` để giữ cho thư mục nguồn `src/` luôn sạch sẽ.
  * Sử dụng `$(dir $@)` để tự động đẻ ra các thư mục con trong `build/`.
* **Biên dịch theo điều kiện (Debug vs Release)**:
  * Sử dụng cú pháp điều kiện `ifeq` và cờ `-DDEBUG` để chuyển đổi giữa chế độ **Release** (`-O3`) và **Debug** (`-g -O0 -DDEBUG`).

### 2. CMake (Meta-Build System)
* **Khái niệm**: CMake là công cụ sinh file build cấp cao. Thay vì viết Makefile thủ công, lập trình viên mô tả cấu trúc dự án trong `CMakeLists.txt` và CMake sẽ tự tạo ra `Makefile` chuẩn hóa.
* **Các câu lệnh cốt lõi**:
  * `cmake_minimum_required(VERSION 3.16)`: Khai báo phiên bản CMake tối thiểu.
  * `project(...)`: Khai báo tên dự án và ngôn ngữ (`CXX`).
  * `include_directories(...)`: Khai báo thư mục chứa file header.
  * `add_executable(...)`: Tạo file thực thi từ danh sách file nguồn.

### 3. Cross-Compilation (Biên dịch chéo cho ARM)
* **Khái niệm**: Biên dịch mã nguồn trên máy tính Host (x86_64) để tạo ra file thực thi nhị phân chạy trên kiến trúc Target khác (ARM 64-bit / AArch64).
* **Toolchain**: Sử dụng `aarch64-linux-gnu-g++`.
* **Static Linking (`-static`)**: Đóng gói toàn bộ thư viện vào file nhị phân để chạy độc lập.
* **Giả lập QEMU**: Sử dụng `qemu-aarch64` để chạy kiểm thử ứng dụng ARM64 ngay trên máy tính x86_64.

---

## 🛠️ Danh Sách Bài Tập Hoàn Thành

### 1. Bài tập 1: Xây dựng Makefile Chuẩn & Nâng Cao
* **Thư mục**: `task_w3/exercise1_makefile/`, `task_w3/exercise1_challenge/`, `task_w3/exercise1_advanced/`
* **Mô tả**: Tự tay thiết kế Makefile build dự án C++ multi-file có phân chia `src/` và `include/`.
* **Cấu trúc Makefile chuẩn**:
  ```makefile
  CXX = g++
  CXXFLAGS = -Wall -Iinclude -MMD -MP
  
  SRCS = $(wildcard src/*.cpp)
  OBJS = $(SRCS:src/%.cpp=build/%.o)
  DEPS = $(OBJS:.o=.d)
  
  app: $(OBJS)
  	$(CXX) $(CXXFLAGS) $^ -o $@
  
  build/%.o: src/%.cpp
  	mkdir -p build
  	$(CXX) $(CXXFLAGS) -c $< -o $@
  
  clean:
  	rm -rf build app
  
  .PHONY: clean
  -include $(DEPS)
  ```

* **Hướng dẫn Biên dịch & Chạy**:
  ```bash
  cd task_w3/exercise1_makefile
  make        # Biên dịch ứng dụng
  ./app       # Chạy ứng dụng
  make clean  # Xóa sạch thư mục build/ và file app
  ```

---

### 2. Bài tập 2: Cấu hình Build System với CMake
* **Thư mục**: `task_w3/exercise2_cmake/`
* **Mô tả**: Chuyển đổi dự án C++ multi-file sang quản lý bằng `CMakeLists.txt`.
* **Cấu trúc `CMakeLists.txt`**:
  ```cmake
  cmake_minimum_required(VERSION 3.16)
  project(MyCMakeApp LANGUAGES CXX)

  include_directories(include)

  add_executable(app_cmake 
      src/main.cpp 
      src/math_utils.cpp 
      src/string_utils.cpp
  )
  ```

* **Hướng dẫn Biên dịch & Chạy (Quy trình 3 bước chuẩn)**:
  ```bash
  cd task_w3/exercise2_cmake
  mkdir build && cd build
  cmake ..
  make
  ./app_cmake
  ```

---

### 3. Bài tập 3: Biên dịch chéo cho Kiến trúc ARM (ARM64)
* **Thư mục**: `task_w3/exercise3_cross_compile/`
* **Mô tả**: Biên dịch file `hello.cpp` cho chip ARM 64-bit và kiểm thử bằng QEMU.
* **Các lệnh thực thi**:
  ```bash
  cd task_w3/exercise3_cross_compile

  # 1. Biên dịch chéo tĩnh cho ARM64
  aarch64-linux-gnu-g++ -static -o hello_arm_static hello.cpp

  # 2. Kiểm tra định dạng nhị phân bằng lệnh file
  file hello_arm_static
  # Kết quả: ELF 64-bit LSB statically linked, ARM aarch64...

  # 3. Giả lập và chạy thử bằng QEMU
  qemu-aarch64 ./hello_arm_static
  # Kết quả in ra:
  # Hello, ARM World!
  # Compiled for 64-bit ARM (AArch64)
  ```

---

> [!NOTE]
> Tất cả các bài tập trên đã được kiểm thử biên dịch thành công 100% trên môi trường Linux Ubuntu/Debian.
