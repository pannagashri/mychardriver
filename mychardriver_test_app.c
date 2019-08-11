#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#define MYCHARFLUSH _IO('R', 0x08)

int main (int argc, char**argv)
{
    int fd = 0;
    size_t ret = 0;
    char buf[1024];
    if (argc != 2) {
        printf("Expecting command line argument: filename to be opened\n");
        return -1;
    }

    if ( (fd = open(argv[1], O_RDWR | O_CREAT)) < 0) {
        printf("Could not open the file %s : Returned error %d\n", argv[1], fd);
        return -1;
    }
    printf("Successfully opened file %s for RW..! FD obtained is %d\n", argv[1], fd);

    ret = write(fd, "Hello from user", strlen("hello from user"));
    if (ret <= 0) {
        printf("Could not write to the file: Returned error %ld\n", ret);
        return -1;
    }
    printf("Wrote %ld bytes\n", ret);
    
    ret = 0;
    if ( (ret = read(fd, buf, 10)) <= 0) {
        printf("Could not read the file: Returned error %ld\n", ret);
        return -1;
    }
    buf[ret] = '\0';
    printf("Read %ld bytes..!\nChars read: %s\n", ret, buf);

    ioctl(fd, MYCHARFLUSH, NULL);

    printf("Trying to read after a flush...\n");
    ret = 0;
    if ( (ret = read(fd, buf, 10)) <= 0) {
        printf("Could not read the file: Returned error %ld\n", ret);
        return -1;
    }
    buf[ret] = '\0';
    printf("Read %ld bytes..!\nChars read: %s\n", ret, buf);
    printf("Closing file now..\n");
    close(fd);
    printf("File closed..!\n");
    return 0;
}
