# Báo Cáo Tuần 6: Linux Character Device Driver & Thực Hành Trên BeagleBone Black

## 1. Lý Thuyết: Linux Character Device Driver

### 1.1. Device Driver là gì?
- **Device Driver (Trình điều khiển thiết bị)** là một phần mềm chạy trong Kernel Space, đóng vai trò trung gian giữa phần cứng và các ứng dụng ở User Space.
- Driver cung cấp một giao diện chuẩn (thông qua các file trong `/dev/`) để ứng dụng có thể giao tiếp với thiết bị mà không cần biết chi tiết phần cứng bên dưới.
- Trong Linux, mọi thứ đều là file ("Everything is a file"), nên thiết bị phần cứng cũng được biểu diễn dưới dạng file đặc biệt trong thư mục `/dev/`.

### 1.2. Phân loại Device Driver trong Linux
Linux chia Device Driver thành 3 loại chính:

| Loại | Mô tả | Ví dụ | Đặc điểm truy cập |
|------|--------|-------|-------------------|
| **Character Device** | Truyền dữ liệu tuần tự (từng byte) | UART, Keyboard, Sensor, GPIO | Đọc/ghi tuần tự, không có buffer cache |
| **Block Device** | Truyền dữ liệu theo khối (block) | Ổ cứng HDD/SSD, USB Storage, eMMC | Đọc/ghi ngẫu nhiên, có buffer cache |
| **Network Device** | Truyền dữ liệu qua giao diện mạng | Ethernet (eth0), WiFi (wlan0) | Không có file trong `/dev/`, dùng socket API |

### 1.3. Character Device Driver chi tiết

#### 1.3.1. Major Number & Minor Number
- Mỗi thiết bị trong `/dev/` được đại diện bởi cặp số **Major Number** và **Minor Number**.
- **Major Number**: Xác định loại driver nào quản lý thiết bị. Ví dụ: tất cả thiết bị serial (ttyS0, ttyS1...) dùng chung Major = 4.
- **Minor Number**: Phân biệt các thiết bị khác nhau do cùng một driver quản lý. Ví dụ: ttyS0 có Minor = 64, ttyS1 có Minor = 65.
- Có thể kiểm tra bằng lệnh: `ls -l /dev/` (cột số thứ 5 và 6 là Major và Minor).

#### 1.3.2. Cấp phát Major/Minor Number
Có 2 cách cấp phát:
- **Tĩnh (Static):** Dùng `register_chrdev_region(dev_t first, unsigned count, char *name)` - tự chọn một số Major cố định. Rủi ro: có thể bị trùng với driver khác.
- **Động (Dynamic):** Dùng `alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, char *name)` - Kernel tự cấp một Major chưa dùng. **Đây là cách được khuyến nghị.**

#### 1.3.3. Cấu trúc `struct file_operations` (fops)
- Đây là bảng ánh xạ (vtable) cho phép Kernel biết khi User Space gọi `open()`, `read()`, `write()`, `close()`... thì sẽ gọi vào hàm nào trong driver.
- Các trường quan trọng:
  - `.owner = THIS_MODULE` – Đảm bảo module không bị unload khi đang được sử dụng.
  - `.open` – Được gọi khi User Space mở file thiết bị (`open("/dev/xxx", ...)`).
  - `.release` – Được gọi khi User Space đóng file thiết bị (`close(fd)`).
  - `.read` – Được gọi khi User Space đọc dữ liệu (`read(fd, buf, len)`).
  - `.write` – Được gọi khi User Space ghi dữ liệu (`write(fd, buf, len)`).

#### 1.3.4. Cấu trúc `struct cdev`
- `struct cdev` đại diện cho một Character Device trong Kernel.
- Quy trình đăng ký:
  1. `cdev_init(&cdev, &fops)` – Khởi tạo và gán bảng fops.
  2. `cdev_add(&cdev, dev_num, count)` – Đăng ký cdev vào Kernel, liên kết với Major/Minor đã cấp phát.
  3. `cdev_del(&cdev)` – Gỡ cdev khi module bị unload.

#### 1.3.5. Tự động tạo Device Node (`/dev/xxx`)
Thay vì phải chạy lệnh `mknod` thủ công, driver có thể tự tạo file thiết bị bằng cách:
1. `class_create(THIS_MODULE, "class_name")` – Tạo một class thiết bị trong `/sys/class/`.
2. `device_create(class, parent, dev_num, NULL, "device_name")` – Tự động tạo file `/dev/device_name` thông qua cơ chế `udev`/`mdev`.
3. Khi cleanup: `device_destroy()` → `class_destroy()`.

#### 1.3.6. Truyền dữ liệu giữa Kernel Space và User Space
- **`copy_to_user(void __user *to, const void *from, unsigned long n)`**: Copy dữ liệu từ kernel buffer sang user buffer (dùng trong hàm `read`).
- **`copy_from_user(void *to, const void __user *from, unsigned long n)`**: Copy dữ liệu từ user buffer vào kernel buffer (dùng trong hàm `write`).
- **Tại sao không dùng `memcpy`?** Vì User Space và Kernel Space nằm ở hai vùng nhớ khác nhau, có cơ chế bảo vệ riêng. `copy_to/from_user` kiểm tra tính hợp lệ của con trỏ User Space trước khi truy cập, tránh crash kernel.

### 1.4. Vòng đời của một Kernel Module
```
insmod main.ko
    │
    ▼
module_init() ──► alloc_chrdev_region() ──► cdev_init() + cdev_add()
    │                                            │
    ▼                                            ▼
class_create() ──► device_create() ──► /dev/char_device xuất hiện
    │
    ▼
[Module đang chạy - Sẵn sàng nhận open/read/write/close từ User Space]
    │
    ▼
rmmod main
    │
    ▼
module_exit() ──► device_destroy() ──► class_destroy()
    │                                        │
    ▼                                        ▼
cdev_del() ──► unregister_chrdev_region() ──► Module được gỡ hoàn toàn
```

### 1.5. Cross-Compilation (Biên dịch chéo)
- **Khái niệm:** Biên dịch code trên máy tính (Host - x86_64) để tạo ra file chạy được trên thiết bị nhúng (Target - ARM 32-bit BeagleBone Black).
- **Toolchain:** Bộ công cụ biên dịch chéo `arm-linux-gnueabihf-gcc` (GNU ARM Hard Float).
- **Kernel Headers:** Để build Kernel Module cho BeagleBone Black, cần có bộ kernel headers tương ứng với phiên bản kernel đang chạy trên board (`5.10.168-ti-r72`). Headers được đặt tại `~/bbb_headers/usr/src/linux-headers-5.10.168-ti-r72`.

---

## 2. Thực Hành: Viết Character Device Driver & Chạy Trên BeagleBone Black

### 2.1. Mô tả bài tập
Viết một **Linux Character Device Driver** hoàn chỉnh với các chức năng:
- Cấp phát động Major/Minor Number.
- Đăng ký `cdev` và gán `file_operations`.
- Tự động tạo device node `/dev/char_device`.
- Hỗ trợ thao tác `open`, `read`, `write`, `close` từ User Space.
- Viết chương trình User Space (`user_test.c`) để kiểm tra driver.
- Cross-compile toàn bộ cho kiến trúc **ARM 32-bit** và chạy trên **BeagleBone Black**.

### 2.2. Cấu trúc thư mục dự án
```
taskw6/
├── main.c           # Mã nguồn Kernel Module (Character Device Driver)
├── user_test.c      # Chương trình User Space để test driver
├── Makefile         # File build: cross-compile module + user app
└── build/           # Thư mục chứa các file output sau khi build
    ├── main.ko      # Kernel Module đã biên dịch (ARM 32-bit)
    ├── user_test    # Chương trình test đã biên dịch (ARM 32-bit, static)
    ├── main.o       # Object file
    ├── main.mod.c   # Module metadata tự sinh bởi Kbuild
    └── ...          # Các file phụ trợ khác
```

### 2.3. Giải thích code Kernel Module (`main.c`)

#### Bước 1: Include các header cần thiết và khai báo thông tin module
```c
#include <linux/init.h>      // Cho __init và __exit
#include <linux/module.h>    // Thư viện bắt buộc cho mọi Kernel Module
#include <linux/fs.h>        // Cho alloc_chrdev_region, struct file_operations
#include <linux/cdev.h>      // Cho cdev_init, cdev_add
#include <linux/device.h>    // Cho class_create, device_create
#include <linux/uaccess.h>   // Cho copy_to_user, copy_from_user

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Khanh");
MODULE_DESCRIPTION("A simple Linux Character Device Driver");
```

#### Bước 2: Khai báo biến toàn cục
```c
static dev_t dev_num;                    // Lưu Major/Minor number
static struct cdev my_cdev;              // Cấu trúc character device
static struct class *my_class = NULL;    // Class thiết bị
static struct device *my_device = NULL;  // Device node

static char kernel_buffer[1024];         // Buffer lưu dữ liệu trong Kernel
static size_t buffer_data_len = 0;       // Kích thước dữ liệu hiện tại trong buffer
```

#### Bước 3: Implement các hàm file_operations
- **`dev_open`**: Log khi thiết bị được mở.
- **`dev_release`**: Log khi thiết bị được đóng.
- **`dev_read`**: Dùng `copy_to_user()` để gửi dữ liệu từ `kernel_buffer` về User Space. Hỗ trợ offset để đọc đúng vị trí, trả về 0 khi hết dữ liệu (EOF).
- **`dev_write`**: Dùng `copy_from_user()` để nhận dữ liệu từ User Space vào `kernel_buffer`. Giới hạn tối đa `BUF_SIZE - 1` byte và thêm null terminator.

#### Bước 4: Hàm khởi tạo module (`__init`)
Thực hiện tuần tự 4 bước, kèm xử lý lỗi theo mô hình **goto cleanup** (reverse order cleanup):
1. `alloc_chrdev_region()` – Cấp phát Major/Minor động.
2. `cdev_init()` + `cdev_add()` – Đăng ký cdev vào kernel.
3. `class_create()` – Tạo class `/sys/class/char_class`.
4. `device_create()` – Tạo device node `/dev/char_device`.

> **Lưu ý:** Mô hình xử lý lỗi dùng `goto` là **best practice** trong Kernel, vì đảm bảo cleanup ngược thứ tự khởi tạo, tránh rò rỉ tài nguyên.

#### Bước 5: Hàm dọn dẹp module (`__exit`)
Giải phóng tài nguyên theo **thứ tự ngược** so với init:
```c
device_destroy() → class_destroy() → cdev_del() → unregister_chrdev_region()
```

### 2.4. Giải thích code User Space Test (`user_test.c`)
Chương trình User Space thực hiện kiểm tra driver theo 3 bước:
1. **Mở thiết bị:** `open("/dev/char_device", O_RDWR)` – Mở file thiết bị với quyền đọc/ghi.
2. **Ghi dữ liệu:** `write(fd, "Hello! This is data test from User Space", ...)` – Gửi chuỗi test xuống driver.
3. **Đọc dữ liệu:** Đóng rồi mở lại thiết bị để reset file offset, sau đó `read()` để đọc lại dữ liệu đã ghi và so sánh.

### 2.5. Giải thích Makefile (Cross-Compilation)
```makefile
# Cấu hình biên dịch chéo cho ARM 32-bit (BeagleBone Black)
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

# Đường dẫn Kernel Headers cho BBB (version 5.10.168-ti-r72)
KDIR := $(HOME)/bbb_headers/usr/src/linux-headers-5.10.168-ti-r72
```

- **Build Kernel Module:** Gọi hệ thống Kbuild của kernel (`make -C $(KDIR) M=$(PWD) modules`). Hệ thống Kbuild sẽ đọc `obj-m += main.o`, biên dịch `main.c` thành `main.ko`.
- **Build User Test:** Dùng cross-compiler trực tiếp (`arm-linux-gnueabihf-gcc`), build dạng **static** (`-static`) để không phụ thuộc vào thư viện trên board.
- **Tổ chức output:** Tất cả artifact được chuyển vào thư mục `build/` cho gọn gàng.

### 2.6. Các bước chạy trên BeagleBone Black

**Bước 1: Cross-compile trên máy Host (Ubuntu x86_64)**
```bash
cd taskw6/
make clean
make all
# Output: build/main.ko và build/user_test
```

**Bước 2: Copy file sang BeagleBone Black qua SCP**
```bash
scp build/main.ko build/user_test debian@<BBB_IP>:/home/debian/
```

**Bước 3: SSH vào BeagleBone Black và load module**
```bash
ssh debian@<BBB_IP>

# Nạp Kernel Module
sudo insmod main.ko

# Kiểm tra module đã load thành công
lsmod | grep main

# Kiểm tra device node đã được tạo
ls -l /dev/char_device

# Xem log kernel
dmesg | tail -n 5
```
**Kết quả dmesg kỳ vọng:**
```text
char_device: Registered with Major = 243, Minor = 0
char_device: Device node /dev/char_device created successfully
```

**Bước 4: Chạy chương trình User Space test**
```bash
# Cấp quyền thực thi
chmod +x user_test

# Chạy chương trình test
sudo ./user_test
```
**Kết quả kỳ vọng:**
```text
Successfully opened device! FD = 3
Writing to device: "Hello! This is data test from User Space"
Successfully written 40 bytes.
Successfully read 40 bytes.
Read content: "Hello! This is data test from User Space"
```

**Bước 5: Kiểm tra log Kernel sau khi test**
```bash
dmesg | tail -n 10
```
**Kết quả kỳ vọng:**
```text
char_device: Device opened
char_device: Received 40 bytes from user space: Hello! This is data test from User Space
char_device: Device closed
char_device: Device opened
char_device: Sent 40 bytes to user space
char_device: Device closed
```

**Bước 6: Gỡ module khi hoàn tất**
```bash
sudo rmmod main
dmesg | tail -n 1
# char_device: Unregistered device driver
```

---

