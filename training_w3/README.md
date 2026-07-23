# Báo Cáo Thực Hành Tuần 3: Build System (Makefile, CMake) & Cross-Compilation

Báo cáo tổng hợp chi tiết kết quả thực hành, học tập và cấu hình hệ thống xây dựng dự án (Build System) trong Tuần 3, tập trung vào **Makefile Căn bản & Nâng cao**, **Meta-Build System CMake (Đa module/Thư viện tĩnh)** và **Biên dịch chéo (Cross-Compilation cho ARM64)**.

---

## Cấu Trúc Tổng Quan Thư Mục Bài Tập

Các nội dung thực hành được chia làm 3 phần độc lập nằm trong thư mục [task_w3](file:///home/khanh/Workspace_company/train_w1/training_w3/task_w3/):

```text
training_w3/
├── task_w3/
│   ├── makefile/          <- Bài tập 1: Xây dựng Makefile C++ đa thư mục, tự động phụ thuộc và đa chế độ build
│   ├── task_cmake/        <- Bài tập 2: Quản lý dự án đa module, đóng gói thư viện tĩnh (Static Library) qua CMake
│   └── task_hello_arm/    <- Bài tập 3: Biên dịch chéo cho vi xử lý ARM 64-bit và giả lập bằng QEMU
└── README.md              <- Báo cáo tổng quát này (File này)
```

---

## PHẦN 1: MAKEFILE CĂN BẢN & NÂNG CAO (`task_w3/makefile`)

Nội dung phần này tập trung nghiên cứu cơ chế hoạt động của Make, cách viết quy tắc tự động hóa và quản lý build tối ưu. Chi tiết xem tại báo cáo riêng: [makefile/README.md](file:///home/khanh/Workspace_company/train_w1/training_w3/task_w3/makefile/README.md).

### 1. Các Kiến Thức Cốt Lõi Đã Áp Dụng
* **Cấu trúc Quy tắc (Rule Syntax):** Bao gồm `target` (mục tiêu cần tạo), `prerequisites` (các file phụ thuộc) và `recipe` (lệnh shell thực thi, bắt buộc thụt đầu dòng bằng **TAB**).
* **Biến tự động (Automatic Variables):**
  * `$@`: Tên target hiện tại.
  * `$<`: File phụ thuộc (prerequisite) đầu tiên.
  * `$^`: Tất cả các file phụ thuộc (không lặp lại).
* **Quy tắc mẫu (Pattern Rules `%`):** Ví dụ `build/%.o: src/%.cpp` giúp tự động định nghĩa quy tắc biên dịch mọi file `.cpp` thành file đối tượng `.o` tương ứng.
* **Tự động theo dõi file Header (Auto-Dependency Tracking):**
  * Sử dụng cờ biên dịch `-MMD -MP` giúp trình biên dịch `g++` tự động xuất file `.d` mô tả các file header `.hpp` được include.
  * Sử dụng `-include $(DEPS)` để nạp các file `.d` vào Makefile. Nhờ đó, chương trình tự động dịch lại chính xác các file `.o` liên quan khi một file `.hpp` thay đổi.
* **Mục tiêu ảo (.PHONY):** Khai báo `.PHONY: all clean run` để thông báo cho Make biết đây là các hành động ảo, tránh xung đột nếu trong thư mục có file cùng tên.

### 2. Biên Dịch Đa Chế Độ (Debug & Release)
Makefile hỗ trợ tham số `BUILD` từ môi trường (mặc định là `release` nếu chưa cấu hình):
* **Chế độ Release** (`make BUILD=release`): Sử dụng cờ `-O3 -DNDEBUG` để tối ưu hóa mã nguồn, đạt tốc độ chạy cao nhất và tắt các dòng debug log. Output là file `app`.
* **Chế độ Debug** (`make BUILD=debug`): Sử dụng cờ `-g -O0 -DDEBUG` để giữ lại các thông tin phân tích dòng lệnh (symbols), tắt tối ưu hóa để dễ dàng debug lỗi. Output là file `app_debug`.

### 3. Quy Trình Sử Dụng Lệnh
Thao tác trong thư mục [makefile](file:///home/khanh/Workspace_company/train_w1/training_w3/task_w3/makefile/):
```bash
make                 # Biên dịch chế độ Release (mặc định) -> sinh file "app"
make BUILD=debug     # Biên dịch chế độ Debug -> sinh file "app_debug"
make run             # Biên dịch và chạy ngay file app
make run BUILD=debug # Biên dịch và chạy ngay file app_debug
make clean           # Dọn dẹp toàn bộ file trung gian (.o, .d) và các file chạy
```

---

## PHẦN 2: QUẢN LÝ DỰ ÁN VỚI CMAKE (`task_w3/task_cmake`)

Nội dung phần này hướng dẫn chuyển đổi từ Makefile thủ công sang sử dụng **CMake (Meta-Build System)** để quản lý dự án C++ đa module chuyên nghiệp. Chi tiết xem tại báo cáo riêng: [task_cmake/README.md](file:///home/khanh/Workspace_company/train_w1/training_w3/task_w3/task_cmake/README.md).

### 1. Kiến Thức Lý Thuyết CMake Cốt Lõi
* **Khái niệm Build Generator:** CMake không biên dịch code mà nó sinh cấu hình build (ví dụ như tạo ra file `Makefile` trên Linux hoặc file `.sln` của Visual Studio trên Windows).
* **Out-of-source Build:** Cơ chế tách biệt thư mục chứa mã nguồn gốc với thư mục build chứa sản phẩm biên dịch (`build/`), đảm bảo thư mục nguồn luôn sạch sẽ.
* **Tầm Vực Liên Kết (Link Visibility):**
  * `PRIVATE`: Chỉ liên kết thư viện phụ thuộc để xây dựng chính nó, không truyền dẫn cho các target kế thừa.
  * `PUBLIC`: Liên kết thư viện và truyền dẫn cấu hình (include path, linker flags) cho các target kế thừa sử dụng.
  * `INTERFACE`: Chỉ truyền cấu hình cho target kế thừa mà bản thân nó không cần tự build (dùng cho header-only).

### 2. Thiết Kế Đa Module & Đóng Gói Thư Viện Tĩnh
Dự án được phân chia thành các module và thư viện tĩnh độc lập:
* **Thư viện tĩnh `geometry_lib`:** Đóng gói toàn bộ logic hình học (Circle, Rectangle, Triangle, Shape Factory) kế thừa đa hình từ giao diện lớp cơ sở `Shape`.
* **Thư viện tĩnh `utils_lib`:** Đóng gói module ghi log (`logger`).
* **Ứng dụng chính `geo_app`:** Sử dụng Smart Pointers, Vector, Algorithm sắp xếp đa hình và liên kết (`target_link_libraries`) với 2 thư viện tĩnh trên dưới dạng `PRIVATE`.

### 3. Cú Pháp Cấu Hình `CMakeLists.txt`
```cmake
cmake_minimum_required(VERSION 3.16)
project(AdvancedGeometryApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB_RECURSE GEOMETRY_SRCS "src/geometry/*.cpp")
file(GLOB_RECURSE UTILS_SRCS "src/utils/*.cpp")

add_library(geometry_lib STATIC ${GEOMETRY_SRCS})
add_library(utils_lib STATIC ${UTILS_SRCS})

add_executable(geo_app src/main.cpp)
target_link_libraries(geo_app PRIVATE geometry_lib utils_lib)
```

### 4. Quy Trình Sử Dụng Lệnh
Thao tác trong thư mục [task_cmake](file:///home/khanh/Workspace_company/train_w1/training_w3/task_w3/task_cmake/):
```bash
mkdir -p build && cd build
cmake ..     # Sinh Makefile từ CMakeLists.txt
make         # Biên dịch mã nguồn tạo executable "geo_app"
./geo_app    # Chạy chương trình
```

---

## PHẦN 3: BIÊN DỊCH CHÉO ARM64 & GIẢ LẬP QEMU (`task_w3/task_hello_arm`)

Nội dung phần này hướng dẫn kỹ thuật phát triển phần mềm nhúng: Biên dịch mã nguồn từ máy Host (x86_64) sang chạy trên phần cứng Target cấu trúc ARM64 (aarch64). Chi tiết xem tại báo cáo riêng: [task_hello_arm/README.md](file:///home/khanh/Workspace_company/train_w1/training_w3/task_w3/task_hello_arm/README.md).

### 1. Khái Niệm Biên Dịch Chéo (Cross-Compilation)
* **Host Machine:** Máy tính phát triển (chạy chip Intel/AMD x86_64).
* **Target Machine:** Thiết bị chạy ứng dụng thực tế (board nhúng ARM64 như Raspberry Pi, Jetson Nano, i.MX8).
* **Cross-Compiler:** Trình biên dịch chạy trên Host nhưng sinh mã nhị phân tương thích với Target. Trong bài thực hành sử dụng bộ công cụ `aarch64-linux-gnu-g++`.

### 2. Các Bước Cấu Hình Biên Dịch Chéo
Ứng dụng LED Controller tương tác với Sysfs GPIO trên Linux được build chéo thông qua các lệnh CMake và chạy thử trên trình giả lập CPU QEMU:

```bash
# 1. Cấu hình sinh build files cho ARM64 bằng cách cấu hình compiler chéo
cmake -B build_arm -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++

# 2. Tiến hành build dự án tạo ra nhị phân ARM64
cmake --build build_arm

# 3. Chạy giả lập chương trình ARM64 trên Host x86_64 bằng QEMU
# Flag -L báo cho QEMU thư mục chứa các thư viện liên kết động động của ARM64
qemu-aarch64 -L /usr/aarch64-linux-gnu build_arm/HelloArmApp
```
