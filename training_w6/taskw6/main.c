/*
 * =============================================================================
 *  LINUX CHARACTER DEVICE DRIVER - Trình điều khiển thiết bị ký tự
 * =============================================================================
 *
 *  User space                          Kernel space
 *  ──────────                          ────────────
 *  open("/dev/char_device")  ──────►   dev_open()
 *  write(fd, "hello", 5)    ──────►   dev_write()  ──► lưu vào kernel_buffer
 *  read(fd, buf, 5)         ──────►   dev_read()   ──► copy từ kernel_buffer ra
 *  close(fd)                ──────►   dev_release()
 *
 * =============================================================================
 */

/* ========================== PHẦN INCLUDE THƯ VIỆN ========================== */

/*
 * <linux/init.h>: Cung cấp macro __init và __exit
 *   - __init: đánh dấu hàm chỉ chạy 1 lần khi load module (insmod).
 *             Sau khi chạy xong, kernel sẽ giải phóng vùng nhớ của hàm này.
 *   - __exit: đánh dấu hàm chỉ chạy khi gỡ module (rmmod).
 *             Nếu module được build-in (không phải loadable), hàm này bị bỏ qua.
 */
#include <linux/init.h>

/*
 * <linux/module.h>: Thư viện BẮT BUỘC cho mọi kernel module.
 *   - Cung cấp các macro: MODULE_LICENSE, MODULE_AUTHOR, module_init, module_exit...
 *   - Không có header này thì không thể biên dịch được module.
 */
#include <linux/module.h>

/*
 * <linux/fs.h>: (fs = filesystem) Cung cấp:
 *   - alloc_chrdev_region(): xin cấp số Major/Minor từ kernel
 *   - struct file_operations: bảng chứa con trỏ hàm (open, read, write, release...)
 *     Đây là cầu nối giữa system call từ user space và hàm xử lý trong driver.
 */
#include <linux/fs.h>

/*
 * <linux/cdev.h>: (cdev = character device) Cung cấp:
 *   - struct cdev: đại diện cho 1 character device trong kernel
 *   - cdev_init(): gắn file_operations vào cdev
 *   - cdev_add(): đăng ký cdev vào hệ thống kernel
 */
#include <linux/cdev.h>

/*
 * <linux/device.h>: Cung cấp:
 *   - class_create(): tạo device class hiển thị trong /sys/class/
 *   - device_create(): tạo file thiết bị trong /dev/ (tự động, không cần mknod thủ công)
 *   Nếu không dùng 2 hàm này, sau khi insmod phải tự tay chạy:
 *     mknod /dev/char_device c <major> <minor>
 */
#include <linux/device.h>

/*
 * <linux/uaccess.h>: (uaccess = user access) Cung cấp:
 *   - copy_to_user():   copy dữ liệu từ kernel space → user space (dùng trong read)
 *   - copy_from_user(): copy dữ liệu từ user space → kernel space (dùng trong write)
 *
 *   TẠI SAO không dùng memcpy() trực tiếp?
 *   Vì kernel space và user space nằm ở vùng nhớ riêng biệt, có cơ chế bảo vệ.
 *   copy_to/from_user() sẽ kiểm tra xem con trỏ user space có hợp lệ không
 *   trước khi copy, tránh kernel bị crash do truy cập vùng nhớ không hợp lệ.
 */
#include <linux/uaccess.h>
#include "my_ioctl.h" 
/* ========================== PHẦN ĐỊNH NGHĨA HẰNG SỐ ======================= */

#define DEVICE_NAME "char_device"   /* Tên thiết bị, sẽ hiển thị tại /dev/char_device */
#define CLASS_NAME  "char_class"    /* Tên class, sẽ hiển thị tại /sys/class/char_class */
#define BUF_SIZE    1024            /* Kích thước buffer trong kernel (tối đa 1024 bytes) */

/* ========================== THÔNG TIN MODULE =============================== */
/*
 * Các macro này khai báo metadata cho module.
 * Xem thông tin bằng lệnh: modinfo char_device.ko
 *
 * MODULE_LICENSE("GPL"): BẮT BUỘC! Nếu không có hoặc không phải "GPL",
 *   kernel sẽ cảnh báo "tainted kernel" và một số API sẽ không dùng được.
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Khanh");
MODULE_DESCRIPTION("A simple Linux Character Device Driver");
MODULE_VERSION("1.0");

/* ========================== BIẾN TOÀN CỤC ================================= */

/*
 * dev_t dev_num:
 *   Kiểu dữ liệu 32-bit chứa cặp số (Major number, Minor number).
 *   - Major number (12 bit cao): xác định LOẠI driver (ví dụ: tất cả ổ cứng SATA dùng chung 1 major)
 *   - Minor number (20 bit thấp): xác định THIẾT BỊ CỤ THỂ do driver đó quản lý
 *   Dùng macro MAJOR(dev_num) và MINOR(dev_num) để tách ra.
 */
static dev_t dev_num;

/*
 * struct cdev my_cdev:
 *   Cấu trúc đại diện cho character device trong kernel.
 *   Chứa con trỏ tới file_operations (bảng các hàm xử lý).
 *   Kernel dùng struct này để biết khi user gọi read()/write() thì
 *   phải gọi hàm nào trong driver.
 */
static struct cdev my_cdev;

/*
 * struct class *my_class:
 *   Con trỏ tới device class. Class giúp udev/systemd tự động tạo
 *   file thiết bị trong /dev/ khi module được load.
 *   Hiển thị tại: /sys/class/char_class/
 */
static struct class *my_class = NULL;

/*
 * struct device *my_device:
 *   Con trỏ tới device object. Kết hợp với class ở trên để
 *   tự động tạo /dev/char_device khi insmod.
 */
static struct device *my_device = NULL;

/*
 * kernel_buffer[]: Vùng nhớ trong kernel để lưu trữ dữ liệu.
 *   Khi user write() dữ liệu vào → lưu tại đây.
 *   Khi user read() dữ liệu ra  → copy từ đây gửi cho user.
 *   Tương tự như một "notepad" đơn giản trong kernel.
 *
 * buffer_data_len: Số byte dữ liệu thực tế đang có trong buffer.
 *   (Khác với BUF_SIZE là kích thước tối đa của buffer)
 */
static char kernel_buffer[BUF_SIZE];
static size_t buffer_data_len = 0;

/* ==========================================================================
 *  CÁC HÀM XỬ LÝ FILE OPERATIONS
 * ==========================================================================
 *
 *  Đây là các hàm mà kernel sẽ gọi khi user space thực hiện
 *  các system call tương ứng (open, close, read, write) trên /dev/char_device.
 *
 *  User space gọi     →    Kernel gọi hàm tương ứng
 *  ─────────────────        ────────────────────────
 *  open()              →    dev_open()
 *  close()             →    dev_release()
 *  read()              →    dev_read()
 *  write()             →    dev_write()
 */

/*
 * dev_open() - Được gọi khi user space mở thiết bị: open("/dev/char_device", ...)
 *
 * Tham số:
 *   @inodep: con trỏ tới inode (thông tin file trên đĩa: quyền, owner, kích thước...)
 *   @filep:  con trỏ tới struct file (đại diện cho 1 file đang mở, chứa vị trí đọc/ghi)
 *
 * Ở đây chỉ in log, nhưng trong thực tế có thể dùng để:
 *   - Kiểm tra quyền truy cập
 *   - Khởi tạo phần cứng
 *   - Cấp phát tài nguyên cho phiên làm việc
 *
 * Return 0 = thành công, giá trị âm = lỗi (ví dụ -EBUSY nếu thiết bị đang bận)
 */
static int dev_open(struct inode *inodep, struct file *filep) {
    pr_info("char_device: Device opened\n");
    return 0;
}

/*
 * dev_release() - Được gọi khi user space đóng thiết bị: close(fd)
 *
 * LƯU Ý: Tên là "release" chứ không phải "close" vì kernel chỉ gọi hàm này
 * khi TẤT CẢ các file descriptor tham chiếu đến file này đều đã đóng.
 * (Ví dụ: nếu dup() hoặc fork(), phải đóng hết mới gọi release)
 *
 * Trong thực tế dùng để:
 *   - Giải phóng tài nguyên đã cấp trong open()
 *   - Tắt phần cứng nếu không còn ai dùng
 */
static int dev_release(struct inode *inodep, struct file *filep) {
    pr_info("char_device: Device closed\n");
    return 0;
}

/*
 * dev_read() - Được gọi khi user space đọc dữ liệu: read(fd, buf, count)
 *
 * Nhiệm vụ: Copy dữ liệu từ kernel_buffer → buffer của user space.
 *
 * Tham số:
 *   @filep:  con trỏ tới struct file (file đang mở)
 *   @buffer: con trỏ tới buffer BÊN USER SPACE (nơi nhận dữ liệu)
 *            "__user" là annotation nhắc nhở: đây là con trỏ user space,
 *            KHÔNG được truy cập trực tiếp, phải dùng copy_to_user()
 *   @len:    số byte mà user muốn đọc
 *   @offset: vị trí đọc hiện tại trong file (con trỏ file position)
 *            Kernel tự quản lý giá trị này giữa các lần read() liên tiếp.
 *
 * Return:
 *   > 0: số byte đã đọc thành công
 *   = 0: EOF (End Of File) - đã đọc hết dữ liệu
 *   < 0: mã lỗi (ví dụ -EFAULT nếu copy thất bại)
 */
static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    size_t bytes_to_copy;
    size_t bytes_not_copied;

    /*
     * Kiểm tra: nếu vị trí đọc (offset) đã vượt qua hoặc bằng lượng dữ liệu
     * có trong buffer → không còn gì để đọc → trả về 0 (EOF).
     *
     * Ví dụ: buffer có 5 byte ("hello"), offset = 5 → đã đọc hết.
     * User space nhận được 0 sẽ biết là hết dữ liệu và dừng đọc.
     */
    if (*offset >= buffer_data_len) {
        return 0;
    }

    /*
     * Tính số byte cần copy = tổng dữ liệu - vị trí đã đọc.
     * Ví dụ: buffer có 10 byte, offset = 3 → còn 7 byte chưa đọc.
     *
     * Nhưng nếu user chỉ yêu cầu đọc ít hơn (len < bytes_to_copy),
     * thì chỉ copy đúng số lượng user yêu cầu, tránh tràn buffer user.
     */
    bytes_to_copy = buffer_data_len - *offset;
    if (bytes_to_copy > len) {
        bytes_to_copy = len;
    }

    /*
     * copy_to_user(dest, src, size):
     *   - dest: buffer bên user space (nơi nhận)
     *   - src:  kernel_buffer + offset (nơi gửi, bắt đầu từ vị trí offset)
     *   - size: số byte cần copy
     *
     * Trả về: số byte CHƯA copy được (0 = thành công hoàn toàn).
     * Nếu != 0 nghĩa là địa chỉ user space bị lỗi (invalid pointer) → trả -EFAULT.
     */
    bytes_not_copied = copy_to_user(buffer, kernel_buffer + *offset, bytes_to_copy);
    if (bytes_not_copied != 0) {
        pr_err("char_device: Failed to copy %zu bytes to user space\n", bytes_not_copied);
        return -EFAULT;
    }

    /*
     * Cập nhật offset: lần read() tiếp theo sẽ đọc tiếp từ vị trí mới.
     * Ví dụ: offset = 0, đọc 5 byte → offset = 5. Lần sau đọc từ byte thứ 5.
     */
    *offset += bytes_to_copy;
    pr_info("char_device: Sent %zu bytes to user space\n", bytes_to_copy);
    return bytes_to_copy;
}

/*
 * dev_write() - Được gọi khi user space ghi dữ liệu: write(fd, "hello", 5)
 *
 * Nhiệm vụ: Copy dữ liệu từ buffer user space → kernel_buffer.
 *
 * Tham số: tương tự dev_read(), nhưng chiều dữ liệu ngược lại.
 *   @buffer: con trỏ tới dữ liệu BÊN USER SPACE (nơi gửi)
 *   @len:    số byte user muốn ghi
 *
 * Return: số byte đã ghi thành công, hoặc mã lỗi âm.
 *
 * LƯU Ý: Hàm này GHI ĐÈ toàn bộ dữ liệu cũ (không phải append).
 * Mỗi lần write() mới sẽ xóa dữ liệu write() trước đó.
 */
static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    size_t bytes_to_copy;
    size_t bytes_not_copied;

    /*
     * Giới hạn kích thước: chỉ copy tối đa (BUF_SIZE - 1) byte.
     * Trừ 1 vì cần chừa 1 byte cho ký tự kết thúc chuỗi '\0'.
     * Nếu user gửi nhiều hơn 1023 byte → chỉ lấy 1023 byte đầu, phần còn lại bị cắt.
     */
    bytes_to_copy = (len < BUF_SIZE - 1) ? len : (BUF_SIZE - 1);

    /*
     * copy_from_user(dest, src, size):
     *   - dest: kernel_buffer (nơi nhận, trong kernel)
     *   - src:  buffer từ user space (nơi gửi)
     *   - size: số byte cần copy
     *
     * Trả về: số byte CHƯA copy được (0 = thành công).
     */
    bytes_not_copied = copy_from_user(kernel_buffer, buffer, bytes_to_copy);
    if (bytes_not_copied != 0) {
        pr_err("char_device: Failed to copy %zu bytes from user space\n", bytes_not_copied);
        return -EFAULT;
    }

    /*
     * Thêm ký tự kết thúc chuỗi '\0' và cập nhật độ dài dữ liệu thực tế.
     * Điều này cho phép dùng kernel_buffer như C string (in bằng %s).
     * Đồng thời cập nhật offset để kernel biết đã ghi bao nhiêu byte.
     */
    kernel_buffer[bytes_to_copy] = '\0';
    buffer_data_len = bytes_to_copy;
    *offset += bytes_to_copy;

    pr_info("char_device: Received %zu bytes from user space: %s\n", bytes_to_copy, kernel_buffer);
    return bytes_to_copy;
}

static long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg){
    int val;

    switch(cmd){
        case WR_VALUE:
            // Nhận dữ liệu từ User gửi lên
            if (copy_from_user(&val, (int __user *)arg, sizeof(val))) {
                return -EFAULT;
            }
            pr_info("char_device: IOCTL Received value = %d\n", val);
            break;

        case RD_VALUE:
            val = 500; // Gán giá trị tùy ý
            if (copy_to_user((int __user *)arg, &val, sizeof(val))) {
                return -EFAULT;
            }
            pr_info("char_device: IOCTL Sending value = %d\n", val);
            break;
        default:
            pr_info("char_device: Unknown IOCTL command %u\n", cmd);
            return -ENOTTY;
    }
    return 0;
}

/* ==========================================================================
 *  FILE OPERATIONS TABLE - Bảng ánh xạ system call → hàm xử lý
 * ==========================================================================
 *
 *  Đây là "bảng đăng ký" cho kernel biết: khi user gọi open/read/write/close
 *  trên thiết bị này thì kernel phải gọi hàm nào.
 *
 *  Cú pháp ".tên_field = giá_trị" là Designated Initializer của C99.
 *  Các field không được gán sẽ tự động là NULL (kernel sẽ dùng hành vi mặc định).
 *
 *  Một số field phổ biến khác (chưa dùng ở đây):
 *    .llseek   → xử lý lseek() (thay đổi vị trí đọc/ghi)
 *    .ioctl    → xử lý ioctl() (các lệnh điều khiển đặc biệt)
 *    .mmap     → xử lý mmap() (ánh xạ bộ nhớ thiết bị vào user space)
 */
static struct file_operations fops = {
    .owner   = THIS_MODULE,   /* Gắn module owner để kernel quản lý reference count.
                                  Ngăn module bị rmmod khi file đang mở. */
    .open    = dev_open,      /* open("/dev/char_device") → gọi dev_open() */
    .release = dev_release,   /* close(fd) → gọi dev_release() */
    .read    = dev_read,      /* read(fd, buf, n) → gọi dev_read() */
    .write   = dev_write,     /* write(fd, buf, n) → gọi dev_write() */
    .unlocked_ioctl = dev_ioctl,
};

/* ==========================================================================
 *  HÀM KHỞI TẠO MODULE - Chạy khi: sudo insmod char_device.ko
 * ==========================================================================
 *
 *  __init: Macro đánh dấu hàm này chỉ cần chạy 1 lần khi load module.
 *  Sau khi init xong, kernel sẽ giải phóng vùng nhớ chứa hàm này.
 *
 *  Quy trình khởi tạo gồm 4 bước (theo thứ tự BẮT BUỘC):
 *    1. Xin cấp số Major/Minor
 *    2. Tạo và đăng ký character device (cdev)
 *    3. Tạo device class trong /sys/class/
 *    4. Tạo device node trong /dev/
 *
 *  Nếu bất kỳ bước nào thất bại → phải dọn dẹp các bước trước đó (goto cleanup).
 *  Pattern này gọi là "goto error handling" - rất phổ biến trong kernel Linux.
 */
static int __init char_device_init(void) {
    int ret;

    /*
     * BƯỚC 1: Xin kernel cấp phát Major/Minor number ĐỘNG (dynamic allocation).
     *
     * alloc_chrdev_region(&dev_num, first_minor, count, name):
     *   - &dev_num:     nơi kernel ghi kết quả (Major + Minor) vào
     *   - 0:            Minor number bắt đầu từ 0
     *   - 1:            chỉ xin 1 device (1 Minor number)
     *   - DEVICE_NAME:  tên hiển thị trong /proc/devices
     *
     * Tại sao dùng "động" (dynamic) thay vì "tĩnh" (static)?
     *   - Static: register_chrdev_region() - phải tự chọn Major number,
     *     dễ trùng với driver khác → xung đột.
     *   - Dynamic: alloc_chrdev_region() - kernel tự chọn Major number còn trống → an toàn hơn.
     *
     * Sau bước này, kiểm tra bằng: cat /proc/devices | grep char_device
     */
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("char_device: Failed to allocate major number\n");
        return ret;
    }
    pr_info("char_device: Registered with Major = %d, Minor = %d\n", MAJOR(dev_num), MINOR(dev_num));

    /*
     * BƯỚC 2: Khởi tạo character device (cdev) và đăng ký vào kernel.
     *
     * cdev_init(&my_cdev, &fops):
     *   Gắn bảng file_operations (fops) vào struct cdev.
     *   Từ giờ kernel biết: cdev này dùng hàm nào để xử lý open/read/write/close.
     *
     * cdev_add(&my_cdev, dev_num, 1):
     *   Đăng ký cdev vào kernel với Major/Minor đã được cấp ở Bước 1.
     *   Sau bước này, kernel chính thức "nhận diện" thiết bị và sẵn sàng
     *   chuyển tiếp các system call đến driver.
     *   Tham số 1 = số lượng Minor number liên tiếp (ở đây chỉ 1 thiết bị).
     */
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;
    ret = cdev_add(&my_cdev, dev_num, 1);
    if (ret < 0) {
        pr_err("char_device: Failed to add cdev\n");
        goto unregister_region; /* Thất bại → nhảy xuống dọn dẹp Bước 1 */
    }

    /*
     * BƯỚC 3: Tạo device class trong sysfs (/sys/class/char_class/).
     *
     * class_create(owner, name):
     *   Tạo một "nhóm" (class) cho thiết bị. Mục đích chính: để udev daemon
     *   biết rằng có thiết bị mới cần tạo file trong /dev/.
     *
     * Sau bước này, kiểm tra bằng: ls /sys/class/char_class/
     *
     * IS_ERR(): macro kiểm tra xem con trỏ có phải là mã lỗi không.
     *   Trong kernel, khi hàm trả về con trỏ, lỗi được encode dưới dạng
     *   con trỏ đặc biệt (ví dụ: (void *)-ENOMEM). IS_ERR() phát hiện điều này.
     * PTR_ERR(): chuyển con trỏ lỗi đó thành mã lỗi số nguyên (int).
     */
    my_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(my_class)) {
        pr_err("char_device: Failed to create class\n");
        ret = PTR_ERR(my_class);
        goto del_cdev; /* Thất bại → dọn dẹp Bước 2 và Bước 1 */
    }

    /*
     * BƯỚC 4: Tạo device node trong /dev/ (ví dụ: /dev/char_device).
     *
     * device_create(class, parent, devt, drvdata, fmt, ...):
     *   - my_class: class đã tạo ở Bước 3
     *   - NULL:     không có parent device
     *   - dev_num:  Major/Minor number
     *   - NULL:     không có driver data bổ sung
     *   - DEVICE_NAME: tên file trong /dev/ → "/dev/char_device"
     *
     * Sau bước này, file /dev/char_device sẽ xuất hiện tự động.
     * User space có thể: echo "hello" > /dev/char_device
     *                     cat /dev/char_device
     *
     * Kiểm tra bằng: ls -la /dev/char_device
     */
    my_device = device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        pr_err("char_device: Failed to create device\n");
        ret = PTR_ERR(my_device);
        goto destroy_class; /* Thất bại → dọn dẹp Bước 3, 2, 1 */
    }

    pr_info("char_device: Device node /dev/%s created successfully\n", DEVICE_NAME);
    return 0; /* Thành công! */

    /*
     * PHẦN DỌN DẸP KHI LỖI (Error Cleanup using goto):
     *
     * Pattern "goto cleanup" là cách chuẩn để xử lý lỗi trong kernel Linux.
     * Ý tưởng: khi bước N thất bại, phải hủy các bước N-1, N-2, ... , 1
     * theo thứ tự NGƯỢC LẠI (giống stack: LIFO - Last In First Out).
     *
     * Ví dụ: nếu Bước 3 (class_create) lỗi:
     *   → nhảy đến del_cdev (hủy Bước 2)
     *   → tiếp tục xuống unregister_region (hủy Bước 1)
     *   → return ret (trả mã lỗi)
     */
destroy_class:
    class_destroy(my_class);
del_cdev:
    cdev_del(&my_cdev);
unregister_region:
    unregister_chrdev_region(dev_num, 1);
    return ret;
}

/* ==========================================================================
 *  HÀM THOÁT MODULE - Chạy khi: sudo rmmod char_device
 * ==========================================================================
 *
 *  __exit: Macro đánh dấu hàm này chỉ cần khi gỡ module.
 *
 *  Dọn dẹp theo thứ tự NGƯỢC LẠI với khởi tạo:
 *    init:  cấp Major → tạo cdev → tạo class → tạo device
 *    exit:  hủy device → hủy class → xóa cdev → trả Major
 *
 *  QUAN TRỌNG: Nếu không dọn dẹp đúng cách sẽ gây:
 *    - Memory leak trong kernel (kernel không có garbage collector!)
 *    - "Zombie" device node trong /dev/
 *    - Không thể insmod lại module (Major number bị giữ)
 */
static void __exit char_device_exit(void) {
    device_destroy(my_class, dev_num);        /* Hủy Bước 4: xóa /dev/char_device */
    class_destroy(my_class);                  /* Hủy Bước 3: xóa /sys/class/char_class */
    cdev_del(&my_cdev);                       /* Hủy Bước 2: gỡ cdev khỏi kernel */
    unregister_chrdev_region(dev_num, 1);     /* Hủy Bước 1: trả lại Major/Minor number */
    pr_info("char_device: Unregistered device driver\n");
}

/* ==========================================================================
 *  ĐĂNG KÝ HÀM INIT VÀ EXIT VỚI KERNEL
 * ==========================================================================
 *
 *  module_init(func): đăng ký hàm sẽ chạy khi "insmod" (load module).
 *  module_exit(func): đăng ký hàm sẽ chạy khi "rmmod" (unload module).
 *
 *  Đây là 2 macro BẮT BUỘC. Không có chúng, kernel không biết phải gọi hàm nào
 *  khi load/unload module.
 */
module_init(char_device_init);
module_exit(char_device_exit);