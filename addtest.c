#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

int main(int argc, char *argv[]) {
    int fd;
    fd = open("test.txt", O_CREATE|O_RDWR);
    printf(fd, "I understand Operating system.\n");
    printf(fd, "I understand Operating system.\n");
    printf(fd, "I understand Operating system.\n");
    printf(fd, "I love to work on OS.\n");
    printf(fd, "I love to work on OS.\n");
    printf(fd, "Thanks xv6.\n");
    printf(fd, "thanks xv6.\n");
    printf(fd, "Thanks xv6.\n");
    printf(1, "Test file created to test uniq.c\n");
    close(fd);
    exit();
}
