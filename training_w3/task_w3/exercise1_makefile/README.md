# Bài Tập 1: Thiết Kế Makefile Cho Dự Án C++ Multi-File

Bài tập thực hành thiết kế file `Makefile` tự động hóa quá trình biên dịch dự án C++ đa file nguồn, hỗ trợ tự động quản lý thư mục `build/` và tự động theo dõi file phụ thuộc header (`.d` files).

---

## 📂 Cấu Trúc Dự Án

```text
exercise1_makefile/
├── include/
│   ├── math_utils.hpp
│   └── string_utils.hpp
├── src/
│   ├── main.cpp
│   ├── math_utils.cpp
│   └── string_utils.cpp
├── build/                 <- Tự động tạo khi chạy make
│   ├── main.o, math_utils.o, string_utils.o
│   └── main.d, math_utils.d, string_utils.d
├── Makefile               <- File cấu hình biên dịch
└── app                    <- File thực thi đầu ra
```

---

## 📄 Mã Nguồn Makefile Hoàn Chỉnh

```makefile
# 1. Cấu hình trình biên dịch và các cờ (flags)
CXX = g++
CXXFLAGS = -Wall -Iinclude -MMD -MP

# 2. Định nghĩa danh sách các file trung gian và file phụ thuộc
OBJS = build/main.o build/math_utils.o build/string_utils.o
DEPS = $(OBJS:.o=.d)

# 3. Target mặc định: Liên kết tạo file thực thi 'app'
app: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# 4. Pattern Rule: Biên dịch các file .cpp trong src/ thành .o trong build/
build/%.o: src/%.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 5. Lệnh dọn dẹp các file rác sinh ra
clean:
	rm -rf build app

.PHONY: clean

# 6. Nạp tự động các file phụ thuộc header (.d)
-include $(DEPS)
```

---

## 🔍 Giải Thích Chi Tiết Cú Pháp

| Thành Phần | Cú Pháp / Lệnh | Giải Thích Ý Nghĩa |
| :--- | :--- | :--- |
| **Compiler & Flags** | `CXX = g++`<br>`CXXFLAGS = -Wall -Iinclude -MMD -MP` | Sử dụng `g++`, bật cảnh báo `-Wall`, tìm header trong `include/`, và tự động sinh file `.d` với `-MMD -MP`. |
| **Ký tự tự động** | `$@`<br>`$<`<br>`$^` | `$@`: Tên mục tiêu (Target).<br>`$<`: Tên điều kiện tiên quyết đầu tiên (First prerequisite).<br>`$^`: Tất cả các điều kiện tiên quyết. |
| **Pattern Rule** | `build/%.o: src/%.cpp` | Ký tự đại diện `%` tự động khớp mọi file `.cpp` trong `src/` để dịch thành `.o` trong `build/`. |
| **Auto Dependency** | `-include $(DEPS)` | Nạp danh sách các file `.d` để `make` tự nhận biết khi có thay đổi trong các file header `.hpp`. |
| **Phony Target** | `.PHONY: clean` | Khai báo `clean` là tên mục tiêu hành động giả lập, tránh trùng tên với file thật. |

---

## 💻 Hướng Dẫn Biên Dịch & Chạy

1. **Biên dịch dự án**:
   ```bash
   make
   ```

2. **Chạy ứng dụng**:
   ```bash
   ./app
   ```

3. **Dọn dẹp dự án (Xóa thư mục `build/` và file `app`)**:
   ```bash
   make clean
   ```
