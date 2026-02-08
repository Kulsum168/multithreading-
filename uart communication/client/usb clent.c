#include <stdio.h>
 
#include <stdlib.h>
 
#include <string.h>
 
#include <unistd.h>
 
#include <fcntl.h>
 
#include <termios.h>
 
#include <sys/select.h>
 
#include <errno.h>
 
#define BUFFER_SIZE 1024
 
#define SERIAL_DEVICE "/dev/ttyUSB0"
 
int setup_serial_port(const char *device) {
 
    int fd = open(device, O_RDWR | O_NOCTTY);
 
    if (fd == -1) {
 
        perror("Error opening serial port");
 
        return -1;
 
    }
 
    struct termios options;
 
    if (tcgetattr(fd, &options) != 0) return -1;
 
    cfsetispeed(&options, B9600);
 
    cfsetospeed(&options, B9600);
 
    options.c_cflag &= ~PARENB;
 
    options.c_cflag &= ~CSTOPB;
 
    options.c_cflag &= ~CSIZE;
 
    options.c_cflag |= CS8;
 
    options.c_cflag |= (CLOCAL | CREAD);
 
    options.c_cflag &= ~CRTSCTS;
 
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
 
    options.c_oflag &= ~OPOST;
 
    if (tcsetattr(fd, TCSANOW, &options) != 0) return -1;
 
    tcflush(fd, TCIOFLUSH);
 
    return fd;
 
}
 
int main() {
 
    int serial_fd = setup_serial_port(SERIAL_DEVICE);
 
    if (serial_fd < 0) return EXIT_FAILURE;
 
    printf("--- USB Client (Raw Mode) ---\n");
 
    printf("Connected to %s.\n", SERIAL_DEVICE);
 
    printf("Waiting for data from Server...\n");
 
    fd_set readfds;
 
    char buffer[BUFFER_SIZE];
 
    while (1) {
 
        FD_ZERO(&readfds);
 
        FD_SET(STDIN_FILENO, &readfds);
 
        FD_SET(serial_fd, &readfds);
 
        int max_fd = (serial_fd > STDIN_FILENO) ? serial_fd : STDIN_FILENO;
 
        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0 && errno != EINTR) break;
 
        // Send to Server
 
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
 
            memset(buffer, 0, BUFFER_SIZE);
 
            int n = read(STDIN_FILENO, buffer, BUFFER_SIZE);
 
            if (n > 0) {
 
                write(serial_fd, buffer, n);
 
            }
 
        }
 
        // Receive from Server
 
        if (FD_ISSET(serial_fd, &readfds)) {
 
            memset(buffer, 0, BUFFER_SIZE);
 
            int n = read(serial_fd, buffer, BUFFER_SIZE - 1);
 
            if (n > 0) {
 
                buffer[n] = '\0';
 
                printf("\n[DEBUG] Received %d bytes: %s\n", n, buffer);
 
                fflush(stdout);
 
            }
 
        }
 
    }
 
    close(serial_fd);
 
    return 0;
 
}
 
