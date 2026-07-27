# Báo Cáo Tổng Quan

## Học Phần: Embedded Linux Cơ Bản

### Kiến Thức & Nội Dung Đào Tạo:
* **Hệ thống & Scripting**: Linux File System Hierarchy; Shell scripting (`bash`, `grep`, `awk`, `sed`).
* **Tiến Trình & Tín Hiệu**: Process & Signal management, Process monitoring.
* **Giao Tiếp Lập Trình (IPC)**: Pipe, Unix Domain Socket (`AF_UNIX`), Shared memory.
* **Công Cụ Debugging & Quản Lý Bộ Nhớ**: `gdb`, `valgrind`, `strace`.
* **Tài Liệu Tham Khảo**: *The Linux Command Line* (W. Shotts), Linux man pages.

---

## Danh Sách 3 Bài Tập Đã Hoàn Thành (Tasks)

| STT | Bài Tập (Task) | Mô Tả Ngắn | Đường Dẫn Chi Tiết |
| :---: | :--- | :--- | :--- |
| **1** | **Debug Memory Leak** | Sửa lỗi biên dịch, lỗi `realloc` gây crash và xử lý rò rỉ bộ nhớ đa tầng bằng `Valgrind`. | [task_w4/debug_memoryleak](./task_w4/debug_memoryleak/README.md) |
| **2** | **Shell Script Monitoring** | Viết kịch bản Bash tự động giám sát %CPU/%RAM thời gian thực, ghi log và cảnh báo vượt ngưỡng. | [task_w4/shell_script](./task_w4/shell_script/README.md) |
| **3** | **IPC Unix Domain Socket** | Thiết kế hệ thống Socket Server/Client (C++) giao tiếp gói tin dữ liệu cảm biến nhị phân. | [task_w4/socket](./task_w4/socket/README.md) |

---

## Tóm Tắt Chi Tiết Các Bài Tập

### 1. Task 1: Debug Memory Leak & Quản Lý Bộ Nhớ C ([Chi tiết](./task_w4/debug_memoryleak/README.md))
* **Mục tiêu**: Khắc phục các lỗi biên dịch, sửa lỗi logic gây crash (Segmentation Fault) và loại bỏ triệt để rò rỉ bộ nhớ (Memory Leak) trong dự án C Device Manager.
* **Kết quả đạt được**:
  * **Fix Crash**: Đổi phép so sánh nhầm `==` thành phép gán `=` khi gọi `realloc` danh sách cảm biến (`device->sensorList = realloc(...)`).
  * **Deep Freeing**: Xây dựng luồng giải phóng bộ nhớ đa tầng (`freeSensorContent`, `freeDeviceContent`) cho Struct lồng nhau (`Device` -> `deviceName`, `ipAddress`, `sensorList` -> `sensorName`).
  * **Xác minh**: Đạt sạch 100% rò rỉ bộ nhớ qua kiểm tra **Valgrind Memcheck** (`All heap blocks were freed -- no leaks are possible`).

---

### 2. Task 2: Shell Script Giám Sát Tiến Trình ([Chi tiết](./task_w4/shell_script/README.md))
* **Mục tiêu**: Lập trình kịch bản Bash ([test.sh](./task_w4/shell_script/test.sh)) giám sát mức độ sử dụng tài nguyên hệ thống (%CPU, %RAM) của tiến trình Linux theo thời gian thực (chu kỳ 2s).
* **Kết quả đạt được**:
  * Trích xuất thông số bằng `pgrep`, `ps -p "$PID" -o %cpu= -o %mem=` và loại bỏ PID của chính script (`grep -v "^$$$"`).
  * Sử dụng `awk` xử lý phép so sánh số thực với ngưỡng cảnh báo (`MAX_CPU = 80.0%`, `MAX_MEM = 50.0%`).
  * Tự động ghi nhật ký ra file [monitor.log](./task_w4/shell_script/monitor.log) kèm mốc thời gian chuẩn ISO, xử lý ngoại lệ mượt mà khi tiến trình bị tắt hoặc không tồn tại.

---

### 3. Task 3: Lập Trình IPC Unix Domain Socket C++ ([Chi tiết](./task_w4/socket/README.md))
* **Mục tiêu**: Xây dựng mô hình Server - Client chuẩn C++ OOP giao tiếp trên cùng máy tính qua Unix Domain Socket (`AF_UNIX`, `SOCK_STREAM`).
* **Kết quả đạt được**:
  * **Giao thức**: Đóng gói thông tin cảm biến dạng Binary Struct ([SensorPacket](./task_w4/socket/IpcProtocol.hpp#L13-L20)) truyền qua Socket Node Path `/tmp/embedded_ipc.sock`.
  * **Server/Client**: [UnixClient.cpp](./task_w4/socket/UnixClient.cpp) nạp và gửi gói tin định kỳ 2 giây/lần; [UnixServer.cpp](./task_w4/socket/UnixServer.cpp) lắng nghe (`bind`, `listen`, `accept`), nhận và hiển thị dữ liệu ra màn hình.
  * **Quản lý vòng đời**: Đăng ký Signal Handler `SIGINT` (`Ctrl+C`) để tự động ngắt kết nối (`close`) và dọn dẹp file socket (`unlink`), chống lỗi `Address already in use`.