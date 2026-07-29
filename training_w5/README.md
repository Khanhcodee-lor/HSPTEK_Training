# Báo Cáo Tuần 5: Tổng Quan Kernel Linux & Thực Hành Build Android Kernel

## 1. Lý Thuyết: Tổng Quan Kernel Linux

### 1.1. Kiến trúc Kernel: Monolithic vs Microkernel
- **Monolithic Kernel (Kernel nguyên khối - ví dụ: Linux):** Toàn bộ các dịch vụ lõi của hệ điều hành (quản lý bộ nhớ, lập lịch CPU, driver phần cứng, hệ thống file) đều chạy chung trong một không gian duy nhất (Kernel Space). Ưu điểm là hiệu năng cực kỳ cao do không phải truyền thông điệp qua lại nhiều. Nhược điểm là nếu một driver bị lỗi (panic) thì toàn bộ hệ thống có thể sập theo.
- **Microkernel (Kernel vi lõi):** Kernel chỉ chứa những dịch vụ cơ bản nhất (chủ yếu là truyền thông điệp - IPC, và lập lịch). Các dịch vụ khác như file system, driver chạy ở User Space. Ưu điểm là rất an toàn, lỗi một tiến trình không làm sập máy. Nhược điểm là hiệu năng chậm hơn do chi phí truyền tải thông điệp giữa các phân vùng (IPC overhead).

### 1.2. Kernel Space vs User Space
- **User Space (Không gian người dùng):** Nơi chạy các ứng dụng thông thường (trình duyệt, app Android, word...). Các ứng dụng ở đây bị giới hạn quyền, không được truy cập trực tiếp vào phần cứng hay bộ nhớ của app khác.
- **Kernel Space (Không gian nhân):** Nơi chạy nhân hệ điều hành và các driver. Vùng này có toàn quyền truy cập vào CPU, RAM và mọi phần cứng. Nếu code ở đây có lỗi (như lỗi truy cập vùng nhớ con trỏ null), hệ thống sẽ bị Kernel Panic.

### 1.3. System Call Interface (Giao diện gọi hệ thống)
- Đây là "cửa khẩu" duy nhất để ứng dụng ở User Space yêu cầu Kernel thực hiện các tác vụ phần cứng (như đọc/ghi file, mở mạng, tạo tiến trình).
- Ví dụ: Khi app gọi hàm `read()`, hệ thống sẽ kích hoạt một *ngắt mềm (software interrupt / trap)*, CPU sẽ chuyển từ chế độ User mode sang Kernel mode để thực hiện việc đọc đĩa, sau đó trả dữ liệu về và quay lại User mode.

### 1.4. Kernel Source Tree Layout (Cấu trúc mã nguồn Kernel)
Mã nguồn Linux Kernel (kể cả bản Android GKI) được tổ chức rất quy củ, một số thư mục chính:
- `arch/`: Chứa mã nguồn phụ thuộc vào từng loại CPU (x86, arm, arm64, riscv...).
- `drivers/`: Mã nguồn của các trình điều khiển phần cứng (chiếm dung lượng lớn nhất).
- `fs/`: Hệ thống file (ext4, fat, f2fs...).
- `include/`: Các header file (`.h`) chung cho toàn kernel.
- `kernel/`: Các chức năng lõi như lập lịch tiến trình, quản lý ngắt.
- `mm/`: Quản lý bộ nhớ (Memory Management).
- *Riêng Android Kernel (GKI):* Sẽ có thêm thư mục `build/` (Kleaf/Bazel) để đóng gói và build kernel theo chuẩn hermetic của Google, và thư mục `common-modules/` để chứa các driver ngoại vi (external modules).

### 1.5. Kernel Config & Build (`menuconfig`, `defconfig`)
- **`.config`**: File gốc định nghĩa toàn bộ tính năng nào của Kernel sẽ được biên dịch.
- **`defconfig`**: (Default Configuration) Các cấu hình chuẩn được các hãng cấu hình sẵn (ví dụ: `x86_64_defconfig`, `gki_defconfig`). Lệnh `make defconfig` sẽ tạo ra file `.config` từ các tuỳ chọn mặc định này.
- **`menuconfig`**: Giao diện UI trên terminal (lệnh `make menuconfig`) cho phép tuỳ chỉnh file `.config` trực quan (thêm/bớt tính năng, chọn build thẳng vào kernel `[ * ]` hay build rời thành module `[ M ]`).

---

## 2. Thực Hành: Build Android Kernel & Viết Kernel Module

### 2.1. Checkout AOSP Kernel Source
- Mã nguồn Android Kernel (GKI) được quản lý qua công cụ `repo`.
- Nguồn mã nguồn được checkout từ nhánh chung của Google: `https://android.googlesource.com/kernel/manifest`.
- Hiện tại source đã được checkout thành công tại `~/android-kernel/`.

### 2.2. Kiểm tra file cấu hình `.config`
- Lấy thông tin cấu hình và toolchain qua hệ thống Kleaf của Google.
- Thay vì dùng `make menuconfig` theo kiểu truyền thống, Android 13+ sử dụng hệ thống Bazel (Kleaf). Cấu hình kernel được định nghĩa qua các target Bazel như `//common:kernel_x86_64`.
- File `.config` thực tế được sinh ra và cache bên trong môi trường sandbox của Bazel (`out/bazel/output_user_root/...`).

### 2.3. Build Kernel Module cho mục tiêu Emulator (x86_64)
- **Công cụ:** Kleaf (Bazel wrapper cho hệ thống build Android Kernel).
- **Mục tiêu:** Tạo một External Kernel Module đơn giản (in ra log dmesg) và build tương thích 100% với GKI (Generic Kernel Image).
- **Các bước thực hiện chi tiết:**
  1. Viết code C cho module tại `~/android-kernel/common-modules/hello_test/hello.c`.
  2. Khởi tạo file `Makefile` (chuẩn Kbuild) và `BUILD.bazel` để tích hợp module này vào hệ thống Kleaf. Khai báo rule `kernel_module(name = "hello", ...)`.
  3. *(Fix lỗi môi trường)* Xử lý lỗi C23 Toolchain (`__isoc23_strtol` của thư viện glibc trên Ubuntu 24.04 làm crash `objtool`) bằng cách patch trực tiếp file `tools/lib/subcmd/parse-options.c` trong mã nguồn kernel.
  4. **Thực thi lệnh build bằng Bazel:**
     ```bash
     cd ~/android-kernel
     tools/bazel build //common-modules/hello_test:hello
     ```
  5. Quá trình biên dịch thành công, Bazel áp dụng đầy đủ các cờ bảo mật LTO, CFI của nhân Android. File đầu ra thu được là `bazel-bin/common-modules/hello_test/hello/hello.ko`.

### 2.4. Bật máy ảo (Emulator) và Load Module
Để test module vừa build ra, chúng ta thực hiện các thao tác trên máy ảo theo trình tự sau:

**Bước 1: Bật máy ảo Android từ Terminal**
Sử dụng công cụ `emulator` của Android SDK để kiểm tra danh sách máy ảo và khởi động (nên dùng thêm cờ `-no-snapshot-load` để clean boot, tránh lỗi state cho kernel):
```bash
# Kiểm tra danh sách máy ảo có sẵn
~/Android/Sdk/emulator/emulator -list-avds

# Khởi động máy ảo (ví dụ tên máy ảo là Pixel_7_Root)
~/Android/Sdk/emulator/emulator @Pixel_7_Root -no-snapshot-load
```
*(Giữ nguyên cửa sổ terminal này để máy ảo tiếp tục chạy).*

**Bước 2: Chuẩn bị công cụ ADB và quyền Root**
Mở một cửa sổ Terminal mới và lấy quyền root để có thể thao tác với Kernel:
```bash
# Trỏ đến công cụ adb chuẩn
export PATH=$HOME/Android/Sdk/platform-tools:$PATH

# Chuyển adb sang chế độ root
adb root
```

**Bước 3: Đẩy module vào thiết bị và nạp vào nhân hệ điều hành**
Dùng lệnh `adb push` để copy file `.ko` vào ổ cứng máy ảo và `insmod` để load nó:
```bash
# Đẩy file
adb push ~/android-kernel/bazel-bin/common-modules/hello_test/hello/hello.ko /data/local/tmp/

# Nạp module vào Kernel
adb shell insmod /data/local/tmp/hello.ko
```

**Bước 4: Kiểm tra kết quả hệ thống**
Sử dụng lệnh `dmesg` để đọc log của Kernel Android, qua đó xác nhận xem module đã được nạp thành công hay chưa:
```bash
adb shell "dmesg | tail -n 15"
```
**Kết quả hiển thị:**
```text
[ 6801.445571] === [SUCCESS] Hello World from Android Kernel Module! ===
```
**Kết luận:** Module được build đúng chuẩn KMI (Kernel Module Interface), vượt qua toàn bộ khâu kiểm tra bảo mật (CFI, CRC Symbol versioning, Module_layout) vô cùng nghiêm ngặt của nhân Android. Mọi task thực hành và lý thuyết của Tuần 5 đã hoàn thành xuất sắc!
