# Báo Cáo Makefile

Báo cáo tổng kết quá trình thực hành, xây dựng **Makefile** cho dự án C++ đa thư mục và các kiến thức cốt lõi đã tiếp thu.

---

## 1. Cấu Trúc Dự Án

Dự án được tổ chức theo cấu trúc phân tách rõ ràng giữa mã nguồn (`src/`), file khai báo (`include/`), và thư mục lưu sản phẩm biên dịch (`build/`):

```text
task_w3/makefile/
├── include/              # Chứa các file header (.hpp / .h)
│   ├── core/
│   └── utils/
├── src/                  # Chứa các file mã nguồn (.cpp)
│   ├── core/
│   ├── utils/
│   └── main.cpp
├── build/                # Thư mục tự động tạo chứa file trung gian (.o, .d)
├── app                   # File thực thi bản Release
├── app_debug             # File thực thi bản Debug
├── makefile              # File cấu hình Build System
└── README.md             # Báo cáo chi tiết task
```

---

## 2. Kiến Thức Makefile Đã Học & Áp Dụng

### 2.1. Cấu trúc cơ bản của 1 Rule
* **Rule Syntax**:
  ```makefile
  target: prerequisites
  	recipe
  ```
* **Target**: Sản phẩm cần tạo (`app`, `build/main.o`) hoặc tên hành động ảo (`clean`, `run`).
* **Prerequisites**: Các file điều kiện cần có trước khi thực thi lệnh.
* **Recipe**: Các dòng lệnh Shell thực thi. **Bắt buộc lùi đầu dòng bằng dấu TAB**.

### 2.2. Automatic Variables (Biến tự động)
* `$@`: Đại diện cho tên **Target** hiện tại.
* `$<`: Đại diện cho **Prerequisite đầu tiên**.
* `$^`: Đại diện cho **Tất cả Prerequisites** (không trùng lặp).

### 2.3. Biến & Toán tử gán (Variables & Operators)
* `BUILD ?= release`: Toán tử `?=` chỉ gán giá trị `release` nếu biến `BUILD` **chưa được định nghĩa** từ môi trường/Terminal.
* `+=`: Nối thêm cờ biên dịch vào `CXXFLAGS`.
* `=`: Gán biến theo cơ chế tính toán lại khi gọi.

### 2.4. Biên dịch theo điều kiện (Conditional Compilation)
Sử dụng cấu trúc `ifeq ... else ... endif` để linh hoạt chuyển đổi giữa chế độ **Release** và **Debug**:
* **Release** (`make` / `make BUILD=release`):
  * Cờ biên dịch: `-O3 -DNDEBUG` (tối ưu tốc độ tối đa, tắt log debug).
  * Đầu ra: file thực thi `app`.
* **Debug** (`make BUILD=debug`):
  * Cờ biên dịch: `-g -O0 -DDEBUG` (bật thông tin debug, tắt tối ưu code để dễ soi lỗi).
  * Đầu ra: file thực thi `app_debug`.

### 2.5. Tự động quét file nguồn (Shell & String Functions)
* **`$(shell find src -type f -name "*.cpp")`**: Tự động tìm tất cả các file `.cpp` trong `src/` kể cả ở các thư mục con lồng nhau (`src/core/`, `src/utils/`).
* **Substitution Reference `$(SRCS:src/%.cpp=build/%.o)`**: Tự động chuyển đổi đường dẫn từ `src/xxx.cpp` thành `build/xxx.o`.

### 2.6. Pattern Rules (`%`) & Tự động tạo thư mục con
* **Pattern Rule `build/%.o: src/%.cpp`**: Quy tắc mẫu biên dịch mọi file `.cpp` thành `.o`.
* **`@mkdir -p $(dir $@)`**: Tự động tạo cấu trúc thư mục tương ứng trong `build/` (ví dụ `build/utils/`) trước khi biên dịch file `.o`.

### 2.7. Tự Động Quản Lý Phụ Thuộc (Auto Dependency - `.d` files)
* **Vấn đề**: Khi chỉnh sửa file header (`.hpp`), Make không biết file `.o` nào cần dịch lại nếu không khai báo thủ công.
* **Giải pháp**:
  1. Thêm cờ `-MMD -MP` vào `CXXFLAGS` để `g++` tự động xuất file `.d` chứa danh sách header được `#include`.
  2. Dùng `-include $(DEPS)` để nạp danh sách phụ thuộc vào Makefile.

### 2.8. Phony Targets (`.PHONY`)
Khai báo `.PHONY: all clean run` để báo cho Make biết đây là các **mục tiêu ảo (hành động)**, tránh xung đột nếu trong thư mục vô tình xuất hiện file trùng tên `clean` hay `run`.


---

## 3. Hướng Dẫn Sử Dụng Lệnh Biên Dịch

| Mục đích | Lệnh thực thi | Mô tả |
| :--- | :--- | :--- |
| **Build mặc định (Release)** | `make` | Biên dịch tối ưu tốc độ (`-O3`), xuất file `app` |
| **Build bản Debug** | `make BUILD=debug` | Biên dịch kèm thông tin debug (`-g -O0`), xuất file `app_debug` |
| **Biên dịch & Chạy ngay** | `make run` | Tự động build (nếu chưa có) và khởi chạy file thực thi |
| **Chạy bản Debug** | `make run BUILD=debug` | Build và chạy bản `app_debug` |
| **Xóa sản phẩm build** | `make clean` | Xóa sạch thư mục `build/`, file `app`, và `app_debug` |

---


