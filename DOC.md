# Báo Cáo Training Tuần 1 - Ngôn Ngữ C Cơ Bản

## Mục tiêu training

Tôi viết tài liệu này để tổng hợp kiến thức đã học và kết quả thực hiện task training tuần 1 của công ty, đặc biệt là bài quản lý danh sách sinh viên bằng danh sách liên kết đơn.

## 1. Kiểu dữ liệu, biến, toán tử

Tôi đã học và sử dụng:

- Kiểu dữ liệu cơ bản: int, float, char.
- Khai báo biến, gán giá trị, và phạm vi biến trong hàm.
- Toán tử số học, so sánh, logic, gán.
- Cấu trúc điều khiển if/else, for, while.

Tôi đã áp dụng để nhập dữ liệu, kiểm tra tính hợp lệ, và xử lý các nhánh logic trong chương trình.

## 2. Hàm và đệ quy

Tôi đã học:

- Tách chương trình thành nhiều hàm nhỏ để dễ đọc, dễ bảo trì.
- Truyền tham số theo giá trị và theo địa chỉ.
- Dùng giá trị trả về để báo thành công/thất bại.

Tôi đã viết các hàm như:

- createNode
- nhapMotSinhVien
- themSV
- timMaSV
- xoaSVTheoMa
- sapXepDanhSachTheoDiemGiamDan
- giaiPhongDanhSach

Tôi đã hiểu khái niệm đệ quy và điều kiện dừng, dù bài hiện tại chưa cần áp dụng đệ quy.

## 3. Con trỏ và mảng

Tôi đã học:

- Con trỏ lưu địa chỉ bộ nhớ.
- Toán tử & (lấy địa chỉ) và \* (giải tham chiếu).
- Con trỏ tới con trỏ (Node\*\*) để thay đổi được head trong hàm.
- Mối liên hệ giữa mảng và con trỏ trong C.

Tôi đã áp dụng con trỏ để duyệt danh sách liên kết bằng biến tạm temp mà không làm mất con trỏ đầu danh sách.

## 4. Struct, Union, Enum

Tôi đã học:

- Struct để nhóm nhiều trường dữ liệu liên quan.
- Khác nhau giữa struct và union.
- Enum để biểu diễn tập giá trị hằng số.

Tôi đã áp dụng:

- struct SinhVien để lưu maSV, hoTen, diem.
- struct Node để lưu dữ liệu SinhVien và con trỏ next.

Tôi chưa cần dùng union và enum trong bài này, nhưng tôi đã nắm được mục đích sử dụng.

## 5. Dynamic memory (malloc/free)

Tôi đã học:

- Cấp phát động bằng malloc.
- Kiểm tra NULL sau khi cấp phát.
- Giải phóng bộ nhớ bằng free để tránh rò rỉ bộ nhớ.

Tôi đã áp dụng:

- Tạo node mới bằng malloc trong createNode.
- Kiểm tra cấp phát thất bại và báo lỗi.
- Giải phóng toàn bộ danh sách bằng giaiPhongDanhSach trước khi kết thúc chương trình.

## Kết quả tôi đạt được

Tôi đã hoàn thành các chức năng:

- Nhập n sinh viên và thêm vào cuối danh sách liên kết.
- In danh sách sinh viên.
- Tìm sinh viên theo mã.
- Xóa sinh viên theo mã.
- Sắp xếp danh sách theo điểm giảm dần.
- In danh sách sau khi xóa và sắp xếp.
- Giải phóng bộ nhớ đã cấp phát.
