# Báo Cáo Thực Hành Tuần 2: C Nâng Cao & C++ Cơ Bản

Báo cáo chi tiết kết quả thực hành và học tập tuần 2 về các kiến thức C nâng cao & C++ cơ bản.

---

## Nội Dung Học Tập (Lý Thuyết)
Trong tuần 2, các kiến thức cốt lõi đã được tìm hiểu và áp dụng bao gồm:
* **Preprocessor**: Sử dụng macro và `#include guard` (header guard) để tránh lỗi định nghĩa chồng chéo khi include nhiều file header.
* **C++ OOP (Lập trình hướng đối tượng)**:
  * Khai báo lớp (`class`), phạm vi truy cập (`protected`, `private`, `public`).
  * Tính kế thừa (`inheritance`) và tính đa hình (`polymorphism`) thông qua các phương thức ảo (`virtual function`, `override`).
  * Sử dụng lớp cơ sở trừu tượng và con trỏ lớp cơ sở để quản lý danh sách đa hình.
* **STL (Standard Template Library)**: Sử dụng các container và kiểu dữ liệu chuẩn như `std::vector`, `std::map` (dùng để lưu trữ index hỗ trợ tìm kiếm nhanh $O(\log N)$) và `std::string`.
* **C++ Templates**: Lập trình tổng quát (generic programming) sử dụng Class Template.
* **RAII & Quản lý bộ nhớ**: Tự động giải phóng bộ nhớ động thông qua việc sử dụng con trỏ thông minh (`std::unique_ptr`, `std::make_unique`) thay thế cho con trỏ thường (`raw pointer`).
* **File I/O (Đọc/Ghi File)**: Sử dụng luồng nhập xuất file `std::ifstream` và `std::ofstream` để tự động lưu trữ và phục hồi dữ liệu từ file văn bản.

---

## Danh Sách Bài Tập Hoàn Thành

### 1. Bài tập 1: Xây dựng Mini Library (OOP)
* **Thư mục**: `task_w2/oop/`
* **Mô tả**: Quản lý các loại tài liệu trong thư viện sử dụng lập trình hướng đối tượng.
* **Cấu trúc file**:
  * [Document.hpp](file:///home/khanh/Workspace_company/train_w2/task_w2/oop/Document.hpp): Định nghĩa lớp cơ sở `Document` chứa thông tin chung (ID, tiêu đề, nhà xuất bản, số bản sao) cùng phương thức hiển thị `virtual void display()`.
  * [Book.hpp](file:///home/khanh/Workspace_company/train_w2/task_w2/oop/Book.hpp): Lớp kế thừa từ `Document`, bổ sung thông tin tác giả và số trang.
  * [Journal.hpp](file:///home/khanh/Workspace_company/train_w2/task_w2/oop/Journal.hpp): Lớp kế thừa từ `Document`, bổ sung thông tin số phát hành và tháng phát hành.
  * [main.cpp](file:///home/khanh/Workspace_company/train_w2/task_w2/oop/main.cpp): Điểm chạy chính của chương trình, tiếp nhận các câu lệnh tương tác, quản lý danh sách tài liệu động thông qua `std::vector<std::unique_ptr<Document>>` và hỗ trợ tìm kiếm nhanh $O(\log N)$ qua `std::map<std::string, Document*>`.

* **Hướng dẫn Biên dịch & Chạy**:
  ```bash
  cd task_w2/oop
  g++ -o main main.cpp
  ./main
  ```

* **Các lệnh tương tác**:
  * Thêm sách: `BOOK [ID] [Title] [Publisher] [Author] [Copies] [Pages]`
    *(Ví dụ: `BOOK "01" "Clean Code" "Prentice Hall" "Robert C. Martin" 10 464`)*
  * Thêm tạp chí: `JOURNAL [ID] [Title] [Publisher] [Copies] [IssueNumber] [Month]`
    *(Ví dụ: `JOURNAL "J01" "Tech Journal" "ACM" 20 12 7`)*
  * Tìm kiếm nhanh bằng ID: `SEARCH [ID]`
    *(Ví dụ: `SEARCH "01"`)*
  * Hiển thị danh sách: `DISPLAY`
  * Lưu thủ công vào file: `SAVE` (ghi đè lên file `library.txt`)
  * Tải thủ công từ file: `LOAD` (tải lại toàn bộ dữ liệu từ `library.txt`)
  * Thoát chương trình: `EXIT` (tự động thực hiện lưu dữ liệu vào `library.txt` trước khi thoát)

> [!NOTE]
> Đối với bất kỳ trường dữ liệu kiểu chuỗi nào (như `ID`, `Title`, `Publisher`, `Author`) có chứa khoảng trắng (dấu cách), bạn chỉ cần đặt chuỗi đó trong cặp dấu nháy kép `""` (ví dụ: `"Clean Code"` hoặc `"IEEE Spectrum"`). Chương trình sẽ tự động xử lý tách từ chính xác và lưu trữ vào file văn bản dưới dạng định dạng chuẩn. Nếu chuỗi không chứa khoảng trắng, bạn có thể viết bình thường mà không cần dấu nháy kép.

* **Cơ chế Lưu trữ File (Challenge 3)**:
  * Khi khởi động, chương trình sẽ tự động đọc dữ liệu từ file `library.txt` (nếu có) để khôi phục trạng thái danh sách tài liệu.
  * Khi kết thúc qua lệnh `EXIT`, chương trình tự động lưu danh sách tài liệu hiện hành trở lại file `library.txt`.

---

### 2. Bài tập 2: Thiết kế Stack sử dụng Template C++
* **Thư mục**: `task_w2/stack/`
* **Mô tả**: Tự thiết kế và cài đặt một cấu trúc dữ liệu Stack tổng quát (`generic Stack`) sử dụng C++ Template trên nền container `std::vector`.
* **Cấu trúc file**:
  * [MyStack.hpp](file:///home/khanh/Workspace_company/train_w2/task_w2/stack/MyStack.hpp): Định nghĩa class template `MyStack<T>` hỗ trợ các thao tác cơ bản của Stack với kiểu dữ liệu bất kỳ.
  * [main.cpp](file:///home/khanh/Workspace_company/train_w2/task_w2/stack/main.cpp): Khởi tạo một ngăn xếp kiểu số nguyên `MyStack<int>` và thực thi các câu lệnh điều khiển từ người dùng qua CLI.

* **Hướng dẫn Biên dịch & Chạy**:
  ```bash
  cd task_w2/stack
  g++ -o main main.cpp
  ./main
  ```

* **Các lệnh tương tác**:
  * Đẩy phần tử vào ngăn xếp: `PUSH [Giá_trị]` *(Ví dụ: `PUSH 10`)*
  * Lấy phần tử ra khỏi ngăn xếp: `POP`
  * Xem phần tử trên cùng: `TOP`
  * Xem số lượng phần tử hiện tại: `SIZE`
  * Kiểm tra ngăn xếp rỗng: `isEmpty`
  * In toàn bộ ngăn xếp: `PRINT`
  * Kết thúc chương trình bằng tổ hợp phím: `Ctrl + D` (EOF) hoặc `Ctrl + C`


