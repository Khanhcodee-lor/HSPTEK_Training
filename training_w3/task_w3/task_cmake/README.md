# Thử Thách CMake Nâng Cao: Thiết Kế Đa Module & Thư Viện Tĩnh

Bài tập thiết kế file `CMakeLists.txt` quản lý một hệ thống C++ đa module phức tạp sử dụng **OOP (Đa hình)**, **Factory Pattern**, **Smart Pointers**, **STL Vector/Algorithm** kết hợp với **Thư viện tĩnh (Static Library)** và **Thư viện tiện ích (Utility Module)**.

---

## 📂 Cấu Trúc Dự Án Nâng Cao

```text
task_cmake/
├── include/
│   ├── geometry/
│   │   ├── shape.hpp           <- Lớp cơ sở trừu tượng (Interface)
│   │   ├── circle.hpp          <- Lớp Kế thừa Circle
│   │   ├── rectangle.hpp       <- Lớp Kế thừa Rectangle
│   │   ├── triangle.hpp        <- Lớp Kế thừa Triangle
│   │   └── shape_factory.hpp   <- Factory Pattern
│   └── utils/
│       └── logger.hpp          <- Module Ghi Log
├── src/
│   ├── geometry/
│   │   ├── circle.cpp
│   │   ├── rectangle.cpp
│   │   ├── triangle.cpp
│   │   └── shape_factory.cpp
│   ├── utils/
│   │   └── logger.cpp
│   └── main.cpp                <- Chương trình chính (Dùng std::sort, Lambda, Smart Pointers)
├── CMakeLists.txt              <- Nơi bạn sẽ viết cấu hình CMake
└── README.md
```

---

## 🎯 Yêu Cầu Đề Bài Viết `CMakeLists.txt`

Viết file `CMakeLists.txt` nằm tại [task_cmake/](file:///home/khanh/Workspace_company/train_w1/training_w3/task_w3/task_cmake/) đáp ứng các tiêu chuẩn sản xuất:

1. **Cấu hình dự án**:
   * Phiên bản CMake tối thiểu `3.16`.
   * Tên dự án: `AdvancedGeometryApp`, ngôn ngữ `CXX`.
   * Bắt buộc chuẩn **C++17** (`set(CMAKE_CXX_STANDARD 17)`).

2. **Thư mục Header**:
   * Khai báo thư mục header `include`.

3. **Quét file nguồn tự động với `file(GLOB_RECURSE ...)`**:
   * Tự động quét tất cả các file nguồn C++ trong `src/geometry/` gán cho biến `GEOMETRY_SRCS`.
   * Tự động quét tất cả các file nguồn C++ trong `src/utils/` gán cho biến `UTILS_SRCS`.

4. **Tạo 2 Thư viện riêng biệt**:
   * Tạo Thư viện tĩnh `geometry_lib` từ `GEOMETRY_SRCS`.
   * Tạo Thư viện tĩnh `utils_lib` từ `UTILS_SRCS`.

5. **Tạo File Thực Thi & Liên Kết (`target_link_libraries`)**:
   * Tạo file chạy `geo_app` từ `src/main.cpp`.
   * Liên kết `geo_app` với cả 2 thư viện `geometry_lib` và `utils_lib`.

---

## 💻 Quy Trình Biên Dịch & Chạy Thử

```bash
cd task_w3/task_cmake
mkdir build && cd build
cmake ..
make
./geo_app
```
