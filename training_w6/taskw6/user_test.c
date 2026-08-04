#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/char_device"

int main() {
    int fd;
    char write_buf[] = "Hello! This is data test from User Space";
    char read_buf[1024] = {0};
    ssize_t ret;

    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device (/dev/char_device)");
        return -1;
    }
    printf("Successfully opened device! FD = %d\n", fd);

    // Write to device
    printf("Writing to device: \"%s\"\n", write_buf);
    ret = write(fd, write_buf, strlen(write_buf));
    if (ret < 0) {
        perror("Failed to write to device");
        close(fd);
        return -1;
    }
    printf("Successfully written %zd bytes.\n", ret);

    // Re-open device to reset file offset for reading
    close(fd);
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to reopen the device for reading");
        return -1;
    }

    // Read from device
    ret = read(fd, read_buf, sizeof(read_buf) - 1);
    if (ret < 0) {
        perror("Failed to read from device");
        close(fd);
        return -1;
    }

    printf("Successfully read %zd bytes.\n", ret);
    printf("Read content: \"%s\"\n", read_buf);

    close(fd);
    return 0;
}
