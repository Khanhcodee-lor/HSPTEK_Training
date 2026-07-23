# BÁO CÁO CMAKE

Báo cáo này trình bày thiết kế và cấu hình file `CMakeLists.txt` để quản lý một hệ thống C++ đa module 

---

## Cấu Trúc Dự Án

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
├── CMakeLists.txt              <- File cấu hình CMake
└── README.md                   <- Tài liệu báo cáo (File này)
```

---

## TỔNG QUAN LÝ THUYẾT CMAKE

### 1. CMake Là Gì?
CMake không phải là một trình biên dịch (compiler) hay một công cụ xây dựng trực tiếp (như `make` hay `ninja`). CMake là một **Build System Generator** (Trình tạo hệ thống xây dựng). 
* Nhiệm vụ của nó là đọc các mô tả cấu hình trong file `CMakeLists.txt` và sinh ra cấu hình build tương ứng trên nền tảng mục tiêu (ví dụ: sinh ra `Makefile` cho công cụ `make` trên Linux/macOS, sinh ra file dự án `.sln` cho MSBuild/Visual Studio trên Windows).
* **Out-of-source Build:** Một nguyên tắc quan trọng trong CMake là không bao giờ biên dịch trực tiếp trên thư mục mã nguồn. Thay vào đó, ta tạo một thư mục build độc lập (thường là `build/`). Mọi file đối tượng (`.o`), file cấu hình build trung gian và file thực thi cuối cùng đều được tạo ra ở đây. Điều này giữ cho thư mục chứa code gốc luôn sạch sẽ và dễ quản lý qua Git.

### 2. Giải Thích Ý Nghĩa Các Lệnh Cốt Lõi Trong `CMakeLists.txt`

* `cmake_minimum_required(VERSION 3.16)`
  Quy định phiên bản CMake tối thiểu cần thiết để cấu hình dự án này. Khai báo này giúp đảm bảo tính tương thích của cú pháp và các chính sách quản lý (Policies) của CMake giữa các phiên bản khác nhau.
  
* `project(AdvancedGeometryApp LANGUAGES CXX)`
  Đặt tên cho dự án là `AdvancedGeometryApp` và khai báo ngôn ngữ sử dụng là C++ (`CXX`). Lệnh này sẽ tự động tạo ra một số biến tiện ích như `${PROJECT_NAME}` hay `${PROJECT_SOURCE_DIR}`.

* `set(CMAKE_CXX_STANDARD 17)` & `set(CMAKE_CXX_STANDARD_REQUIRED ON)`
  Cấu hình yêu cầu trình biên dịch (compiler) phải hỗ trợ chuẩn ngôn ngữ **C++17** trở lên và bắt buộc dừng biên dịch (throw error) nếu compiler của hệ thống không đáp ứng được yêu cầu này.

* `include_directories(include)`
  Khai báo thư mục chứa file tiêu đề (header) của dự án. Lệnh này báo cho compiler thêm thư mục `include` vào cờ tìm kiếm đầu vào (`-I`). Nhờ đó, trong code nguồn chúng ta có thể `#include "geometry/shape.hpp"` trực tiếp thay vì phải dùng đường dẫn tương đối phức tạp như `#include "../../include/geometry/shape.hpp"`.

* `file(GLOB_RECURSE GEOMETRY_SRCS "src/geometry/*.cpp")`
  Lệnh quét tệp tin tìm kiếm đệ quy. Nó sẽ tìm tất cả các file có đuôi `.cpp` nằm trong thư mục `src/geometry/` và gán danh sách file tìm được vào biến `GEOMETRY_SRCS`.
  > [!WARNING]
  > **Hạn chế của GLOB:** Việc dùng `GLOB` rất tiện lợi vì ta không cần liệt kê thủ công từng file. Tuy nhiên, điểm yếu của nó là CMake sẽ không tự phát hiện nếu bạn thêm một file nguồn mới vào thư mục trừ khi bạn chạy lại lệnh sinh cấu hình (`cmake ..`). Trong dự án thực tế quy mô lớn, việc liệt kê tường minh từng file nguồn vẫn được khuyến khích hơn.

* `add_library(geometry_lib STATIC ${GEOMETRY_SRCS})`
  Tạo ra một **Thư viện tĩnh (Static Library)** tên là `geometry_lib` từ danh sách các file nguồn chứa trong biến `GEOMETRY_SRCS`. Trên Linux, thư viện này sẽ được biên dịch thành file `libgeometry_lib.a`.

* `add_executable(geo_app src/main.cpp)`
  Khai báo mục tiêu là một file thực thi (Executable) tên là `geo_app` được tạo từ file nguồn chứa hàm main là `src/main.cpp`.

* `target_link_libraries(geo_app PRIVATE geometry_lib utils_lib)`
  Thực hiện quá trình **Liên kết (Linking)** file thực thi `geo_app` với hai thư viện tĩnh là `geometry_lib` và `utils_lib`.

### 3. Phân Biệt Các Phạm Vi Liên Kết (Link Visibility): PRIVATE, PUBLIC, INTERFACE
Trong CMake hiện đại, khi liên kết các thư viện phụ thuộc với nhau thông qua `target_link_libraries`, ta cần xác định tầm vực (visibility) để kiểm soát sự truyền dẫn cấu hình phụ thuộc:
* **`PRIVATE`**: Thư viện đích cần thư viện phụ thuộc để biên dịch chính nó, nhưng các mục tiêu khác liên kết với thư viện đích sẽ không được tự động kế thừa thư viện phụ thuộc này. (Ví dụ: Ứng dụng `geo_app` dùng `geometry_lib` thì liên kết `PRIVATE` là phù hợp vì không có gì kế thừa từ `geo_app` nữa).
* **`PUBLIC`**: Thư viện phụ thuộc vừa được sử dụng để xây dựng thư viện đích, vừa được truyền dẫn ra ngoài cho bất kỳ dự án/mục tiêu nào liên kết với thư viện đích đó.
* **`INTERFACE`**: Thư viện đích không cần thư viện phụ thuộc để build (thường là thư viện chỉ có file header - Header Only Library), nhưng các mục tiêu liên kết với nó bắt buộc phải link với thư viện phụ thuộc này.

---


## Quy Trình Biên Dịch & Chạy Thử

Thực hiện các lệnh sau trên terminal để build và chạy ứng dụng:

```bash
# 1. Truy cập thư mục dự án
cd task_w3/task_cmake

# 2. Tạo thư mục chứa các file build trung gian (Out-of-source Build)
mkdir -p build && cd build

# 3. Sử dụng CMake để sinh hệ thống build (Makefile)
cmake ..

# 4. Biên dịch mã nguồn thành file thực thi
make

# 5. Chạy ứng dụng
./geo_app
```
