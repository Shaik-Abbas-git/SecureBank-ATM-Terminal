/**
 * @file      uart.c
 * @brief     Linux Serial Port Interface via termios
 * @author    Shaik Abbas
 * @date      November 2026
 */

#include <stdint.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "uart.h"

/* Opens and configures the serial port (8N1, Raw Mode) */
int serialOpen(const char *device, const int baud) {
    struct termios options;
    speed_t myBaud;
    int status, fd;

    switch (baud) {
        case 4800:   myBaud = B4800; break;
        case 9600:   myBaud = B9600; break;
        case 19200:  myBaud = B19200; break;
        case 38400:  myBaud = B38400; break;
        case 57600:  myBaud = B57600; break;
        case 115200: myBaud = B115200; break;
        default:     return -2;
    }

    if ((fd = open(device, O_RDWR | O_NOCTTY)) == -1)
        return -1;

    tcgetattr(fd, &options);
    cfmakeraw(&options);
    cfsetispeed(&options, myBaud);
    cfsetospeed(&options, myBaud);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;

    tcsetattr(fd, TCSANOW | TCSAFLUSH, &options);
    usleep(10000);

    return fd;
}

/* Flushes the serial TX and RX buffers */
void serialFlush(const int fd) {
    tcflush(fd, TCIOFLUSH);
}

/* Closes the serial file descriptor */
void serialClose(const int fd) {
    close(fd);
}

/* Sends a single byte over the serial port */
void serialPutchar(const int fd, const unsigned char c) {
    write(fd, &c, 1);
}

/* Blocks to receive a single byte (with a 10s kernel timeout depending on VTIME) */
int serialGetchar(const int fd) {
    uint8_t x;
    if (read(fd, &x, 1) != 1)
        return -1;
    return ((int)x) & 0xFF;
}

/* Transmits a complete null-terminated string */
void serialPutString(const int fd, const char *str) {
    size_t i;
    size_t len = strlen(str);
    for (i = 0; i < len; i++)
        serialPutchar(fd, (unsigned char)str[i]);
}

/* Reads characters until the protocol terminator '$' is found */
int serialGetString(const int fd, char *buf, int buf_size) {
    int idx = 0;
    int c;

    while (idx < buf_size - 1) {
        c = serialGetchar(fd);
        
        if (c == -1) return -1;
        if (c == '$') break;
        
        buf[idx++] = (char)c;
    }

    buf[idx] = '\0';
    return idx;
}
