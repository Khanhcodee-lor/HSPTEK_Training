# Biên Dịch Chéo (Cross-Compile) Cho ARM - Ứng Dụng Hello ARM LED Controller

Dự án này hướng dẫn cách cấu hình, biên dịch chéo (cross-compile) một ứng dụng C++ điều khiển LED đơn (sử dụng giao tiếp Sysfs GPIO trên Linux) từ máy tính Host (x86_64) sang board mạch mục tiêu chạy cấu trúc **ARM 64-bit (aarch64/ARMv8)** và chạy giả lập kiểm thử bằng **QEMU**.

---

## Cấu Trúc Thư Mục Dự Án

```text
task_hello_arm/
├── include/
│   └── led_controller.hpp   # Header định nghĩa Class điều khiển LED
├── src/
│   ├── led_controller.cpp   # Driver điều khiển Sysfs GPIO trên Embedded Linux
│   └── main.cpp             # Ứng dụng điều khiển chớp tắt LED (chứa hàm main)
├── CMakeLists.txt           # File cấu hình build hệ thống bằng CMake
├── run_arm.sh               # Script tự động hóa quá trình build và chạy giả lập
└── README.md                # Tài liệu hướng dẫn (File này)
```

---

## Yêu Cầu Chuẩn Bị Trên Máy Host (Ubuntu/Debian)

Để có thể thực hiện biên dịch chéo và chạy giả lập cho kiến trúc ARM64, bạn cần cài đặt bộ công cụ cross-compiler (toolchain) và emulator (QEMU) trên máy Host Linux:

```bash
# Cập nhật danh sách gói phần mềm
sudo apt update

# Cài đặt trình biên dịch chéo GCC/G++ cho ARM64
sudo apt install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

# Cài đặt trình giả lập QEMU kiến trúc ARM 64-bit
sudo apt install -y qemu-user qemu-user-static

# Cài đặt công cụ xây dựng dự án
sudo apt install -y cmake build-essential
```

---

## Giải Thích Cấu Hình Hệ Thống Build

### 1. Cấu Hình CMake (`CMakeLists.txt`)
Dự án sử dụng CMake để quản lý quá trình biên dịch:
* **Chuẩn C++**: Sử dụng chuẩn `C++17` (`set(CMAKE_CXX_STANDARD 17)`).
* **Định nghĩa thư viện**: Tạo một thư viện tĩnh `led_drive` từ `led_controller.cpp` để tái sử dụng.
* **Định nghĩa file thực thi**: Compile file `main.cpp` và `led_controller.cpp` tạo ra file chạy tên `HelloArmApp`.
* **Link thư viện**: Link với thư viện `pthread` để hỗ trợ đa luồng (cho tác vụ blink).

### 2. File Script Tự Động Hóa (`run_arm.sh`)
Script này bao gồm toàn bộ quy trình biên dịch chéo và chạy thử:
```bash
#!/bin/bash
# 1. Cấu hình sinh Makefile cho ARM64 bằng cách chỉ định compiler chéo
cmake -B build_arm -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++

# 2. Thực hiện build tạo file thực thi
cmake --build build_arm

# 3. Chạy giả lập chương trình ARM64 trên Host x86_64
qemu-aarch64 -L /usr/aarch64-linux-gnu build_arm/HelloArmApp
```

---

## Các Bước Biên Dịch Và Chạy Thử Nghiệm

### Bước 1: Phân Quyền Cho Script Chạy
Trước tiên, hãy cấp quyền thực thi cho file `run_arm.sh`:
```bash
chmod +x run_arm.sh
```

### Bước 2: Thực Thi Quá Trình Build & Giả Lập
Chạy script tự động hóa:
```bash
./run_arm.sh
```

**Kết quả đầu ra mong đợi:**
```text
-- Configuring done (0.0s)
-- Generating done (0.0s)
-- Build files have been written to: .../task_hello_arm/build_arm
[ 40%] Built target led_drive
[100%] Built target HelloArmApp
===========================================
   Hello World!                            
   Embedded Linux ARM - LED Control App    
===========================================

--- Testing Manual Control ---
[ARM LED Driver] Initializing GPIO Pin 18...
[ARM LED Driver] Running in Simulation Mode (No physical Sysfs GPIO detected).
[LED Pin 18] -> HIGH (💡 LED ON)
[LED Pin 18] -> LOW  (⬛ LED OFF)

--- Testing Automatic Blinking ---
[ARM LED Driver] Starting Blink Sequence: 5 times, delay 300ms
[LED Pin 18] -> HIGH (💡 LED ON)
[LED Pin 18] -> LOW  (⬛ LED OFF)
...
[ARM LED Driver] Blink Sequence Completed!

Embedded Linux ARM App Terminated Successfully.
```

### Bước 3: Xác Minh Kiến Trúc File Thực Thi (ARM64)
Để chắc chắn rằng file thực thi đã được biên dịch chéo sang cấu trúc ARM64 chứ không phải x86_64 của máy Host, chạy lệnh:
```bash
file build_arm/HelloArmApp
```

**Kết quả trả về sẽ tương tự như sau:**
```text
build_arm/HelloArmApp: ELF 64-bit LSB executable, ARM aarch64, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-aarch64.so.1, for GNU/Linux 3.7.0, BuildID[sha1]=..., not stripped
```

---
