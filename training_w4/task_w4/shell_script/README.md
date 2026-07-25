# Báo Cáo Task: Shell Script Giám Sát Tiến Trình Hệ Thống (Process Monitoring)

## 1. Mục Tiêu Bài Tập

Báo cáo này tổng hợp chi tiết kiến thức lý thuyết và thực hành của bài tập **Shell Scripting - Giám sát Process** thuộc học phần *Embedded Linux cơ bản*.

### Mục tiêu đạt được:
- Viết kịch bản Bash (`test.sh`) tự động theo dõi liên tục trạng thái hoạt động của một tiến trình (Process) trên hệ điều hành Linux.
- Đo lường chính xác mức độ tiêu thụ tài nguyên hệ thống gồm **% CPU** và **% RAM** của tiến trình theo thời gian thực (chu kỳ 2 giây/lần).
- Đưa ra cảnh báo tức thì ra màn hình/log khi mức tiêu thụ vượt ngưỡng cấu hình (`MAX_CPU = 80.0%`, `MAX_MEM = 50.0%`).
- Xử lý mượt mà các tình huống biên: thiếu tham số đầu vào, tiến trình chưa khởi chạy, hoặc tiến trình bị ngắt đột ngột.

---

## 2. Kiến Thức Kỹ Thuật & Các Câu Lệnh Cụ Thể

### 2.1. Quản Lý Tiến Trình (Process Management) & Biến Đặc Biệt Trong Bash

* **PID (Process ID)**: Số nguyên duy nhất do kernel Linux cấp phát để quản lý mỗi tiến trình đang chạy.
* **Biến đặc biệt `$$`**:
  * Trong Bash, `$$` lưu trữ **PID của chính kịch bản shell hiện tại** đang thực thi.
* **Lệnh `pgrep`**:
  * Cú pháp: `pgrep -f "$TARGET"`
  * Chức năng: Tìm kiếm tất cả PID của các tiến trình đang chạy có dòng lệnh (command line) khớp với từ khóa `$TARGET`.
  * Cờ `-f` (Full command line): Bắt cặp toàn bộ chuỗi lệnh thay vì chỉ tên file thực thi.
* **Lọc PID bản thân bằng `grep -v "^$$$"`**:
  * Khi chạy `./test.sh zsh`, tên kịch bản cũng chứa chuỗi `zsh` hoặc tên tương tự. Để tránh kịch bản tự giám sát chính mình, ta lọc bỏ PID của bản thân:
    * `grep -v`: Loại bỏ các dòng khớp với pattern.
    * `^$$$`: Regex bắt khớp chính xác dòng chỉ chứa duy nhất PID của script (`^` là bắt đầu dòng, `$$` là PID script, `$` là kết thúc dòng).
* **Lệnh `head -n 1`**:
  * Lấy PID đầu tiên tìm thấy trong trường hợp có nhiều tiến trình cùng tên đang chạy song song.

---

### 2.2. Trích Xuất Thông Số Tài Nguyên Hệ Thống bằng `ps`

* **Cú pháp lệnh `ps`**:
  ```bash
  CPU=$(LC_ALL=C ps -p "$PID" -o %cpu= | xargs)
  MEM=$(LC_ALL=C ps -p "$PID" -o %mem= | xargs)
  ```
* **Giải thích chi tiết các thành phần:**
  * `-p "$PID"`: Chỉ định chính xác PID cần truy vấn.
  * `-o %cpu=`: Chỉ xuất cột phần trăm CPU. Dấu `=` ở cuối dùng để **xóa bỏ tiêu đề cột** (header), chỉ lấy giá trị số.
  * `-o %mem=`: Chỉ xuất cột phần trăm bộ nhớ RAM (xóa bỏ header).
  * `LC_ALL=C`: Đặt biến môi trường Locale về chuẩn mặc định (`C` Locale). Điều này đảm bảo số thập phân luôn dùng **dấu chấm `.`** (ví dụ `12.5`), tránh lỗi định dạng ở các hệ thống dùng dấu phẩy `,` (ví dụ `12,5`).
  * `xargs`: Loại bỏ toàn bộ khoảng trắng thừa (leading/trailing spaces) xung quanh kết quả trả về.

---

### 2.3. So Sánh Số Thực (Floating-Point Comparison) bằng `awk`

* **Vấn đề của Bash native**:
  * Trình biên dịch Bash nguyên bản chỉ hỗ trợ phép toán số nguyên (Integer Math). Phép so sánh như `[ "12.5" -gt "80.0" ]` sẽ gây lỗi cú pháp (`integer expression expected`).
* **Giải pháp sử dụng `awk`**:
  * `awk` hỗ trợ xử lý số thực mạnh mẽ.
  * Cú pháp lệnh:
    ```bash
    IS_CPU_HIGH=$(awk -v cpu="$CPU" -v max="$MAX_CPU" 'BEGIN { print (cpu > max) ? 1 : 0 }')
    ```
  * **Giải thích:**
    * `-v cpu="$CPU" -v max="$MAX_CPU"`: Truyền biến từ Shell vào trong môi trường `awk`.
    * `BEGIN { ... }`: Thực thi ngay khối lệnh mà không cần đọc dữ liệu từ file đầu vào.
    * `(cpu > max) ? 1 : 0`: Toán tử điều kiện 3 ngôi (Ternary Operator). Nếu `cpu` lớn hơn `max` trả về `1` (True), ngược lại trả về `0` (False).

---

### 2.4. Cấu Trúc Điều Khiển & Vòng Lặp Trong Shell

* **Kiểm tra tham số đầu vào `$1`**:
  * `TARGET=$1`: Lấy tham số thứ nhất truyền vào kịch bản.
  * `if [ -z "$TARGET" ]; then ... exit 1; fi`: Kiểm tra bằng toán tử `-z` (String length is zero). Nếu người dùng chưa nhập tên tiến trình, in hướng dẫn sử dụng và dừng kịch bản với mã lỗi `exit 1`.
* **Vòng lặp vô hạn `while true`**:
  * Giúp kịch bản lắng nghe và giám sát tiến trình liên tục theo thời gian thực.
* **Tạm dừng chu kỳ bằng `sleep 2`**:
  * Dừng thực thi 2 giây giữa mỗi lần lấy mẫu để tránh gây quá tải CPU cho hệ thống.
* **Ghi dấu thời gian bằng `date`**:
  * `date +"%Y-%m-%d %H:%M:%S"`: Tạo chuỗi thời gian định dạng chuẩn ISO `YYYY-MM-DD HH:MM:SS` để đính kèm vào mỗi dòng log.

---

### 2.5. Đổi Hướng I/O & Chạy Ngầm Tiến Trình (I/O Redirection & Backgrounding)

* **Ghi log ra file**:
  ```bash
  ./test.sh zsh > monitor.log 2>&1 &
  ```
  * `> monitor.log`: Đổi hướng Standard Output (STDOUT - luồng 1) ghi vào file `monitor.log`.
  * `2>&1`: Đổi hướng Standard Error (STDERR - luồng 2) gộp chung vào luồng Standard Output (luồng 1).
  * `&`: Đưa script chạy dưới dạng **Process ngầm (Background Job)**, trả lại quyền điều khiển Terminal cho người dùng.

---

## 3. Cấu Trúc File & Mã Nguồn Script

### Cấu trúc thư mục:
```text
shell_script/
├── test.sh         # Kịch bản Bash giám sát tiến trình
├── monitor.log     # File nhật ký lưu vết giám sát
└── README.md       # Báo cáo chi tiết bài tập
```


---

## 4. Hướng Dẫn Vận Hành

1. **Cấp quyền thực thi:**
   ```bash
   chmod +x test.sh
   ```
2. **Chạy trực tiếp giám sát tiến trình `zsh`:**
   ```bash
   ./test.sh zsh
   ```
3. **Chạy ngầm và lưu log:**
   ```bash
   ./test.sh zsh > monitor.log 2>&1 &
   ```

---


