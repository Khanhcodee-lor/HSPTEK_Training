#include <linux/init.h>      // Cho __init và __exit
#include <linux/module.h>    // Thư viện bắt buộc cho mọi Kernel Module
#include <linux/fs.h>        // Cho alloc_chrdev_region, struct file_operations
#include <linux/cdev.h>      // Cho cdev_init, cdev_add
#include <linux/device.h>    // Cho class_create, device_create
#include <linux/uaccess.h>   // Cho copy_to_user, copy_from_user

#define DEVICE_NAME "char_device"
#define CLASS_NAME  "char_class"
#define BUF_SIZE    1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Khanh");
MODULE_DESCRIPTION("A simple Linux Character Device Driver");
MODULE_VERSION("1.0");

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class = NULL;
static struct device *my_device = NULL;

static char kernel_buffer[BUF_SIZE];
static size_t buffer_data_len = 0;

static int dev_open(struct inode *inodep, struct file *filep) {
    pr_info("char_device: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    pr_info("char_device: Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    size_t bytes_to_copy;
    size_t bytes_not_copied;

    if (*offset >= buffer_data_len) {
        return 0; // EOF
    }

    bytes_to_copy = buffer_data_len - *offset;
    if (bytes_to_copy > len) {
        bytes_to_copy = len;
    }

    bytes_not_copied = copy_to_user(buffer, kernel_buffer + *offset, bytes_to_copy);
    if (bytes_not_copied != 0) {
        pr_err("char_device: Failed to copy %zu bytes to user space\n", bytes_not_copied);
        return -EFAULT;
    }

    *offset += bytes_to_copy;
    pr_info("char_device: Sent %zu bytes to user space\n", bytes_to_copy);
    return bytes_to_copy;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    size_t bytes_to_copy;
    size_t bytes_not_copied;

    bytes_to_copy = (len < BUF_SIZE - 1) ? len : (BUF_SIZE - 1);

    bytes_not_copied = copy_from_user(kernel_buffer, buffer, bytes_to_copy);
    if (bytes_not_copied != 0) {
        pr_err("char_device: Failed to copy %zu bytes from user space\n", bytes_not_copied);
        return -EFAULT;
    }

    kernel_buffer[bytes_to_copy] = '\0';
    buffer_data_len = bytes_to_copy;
    *offset += bytes_to_copy;

    pr_info("char_device: Received %zu bytes from user space: %s\n", bytes_to_copy, kernel_buffer);
    return bytes_to_copy;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int __init char_device_init(void) {
    int ret;

    // 1. Cấp phát động Major/Minor number
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("char_device: Failed to allocate major number\n");
        return ret;
    }
    pr_info("char_device: Registered with Major = %d, Minor = %d\n", MAJOR(dev_num), MINOR(dev_num));

    // 2. Khởi tạo cdev và add vào kernel
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;
    ret = cdev_add(&my_cdev, dev_num, 1);
    if (ret < 0) {
        pr_err("char_device: Failed to add cdev\n");
        goto unregister_region;
    }

    // 3. Tạo class thiết bị (/sys/class/char_class)
    my_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(my_class)) {
        pr_err("char_device: Failed to create class\n");
        ret = PTR_ERR(my_class); 
        goto del_cdev;
    }

    // 4. Tạo device node trong /dev (/dev/char_device)
    my_device = device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        pr_err("char_device: Failed to create device\n");
        ret = PTR_ERR(my_device);
        goto destroy_class;
    }

    pr_info("char_device: Device node /dev/%s created successfully\n", DEVICE_NAME);
    return 0;

destroy_class:
    class_destroy(my_class);
del_cdev:
    cdev_del(&my_cdev);
unregister_region:
    unregister_chrdev_region(dev_num, 1);
    return ret;
}

static void __exit char_device_exit(void) {
    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("char_device: Unregistered device driver\n");
}

module_init(char_device_init);
module_exit(char_device_exit);