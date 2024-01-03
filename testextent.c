#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "stat.h"
#include "fs.h"

int main(int argc, char *argv[]) {
    unlink("test");
    int fd2 = open("test", O_CREATE | O_RDWR | O_EXTENT);
    if (fd2 < 0) {
        printf(1, "test: cannot open testett file for writing\n");
        exit();
    }

    char string2[] = "TEST 2: Checking READ/WRITE O_EXTENT flags";
    int cc = write(fd2, string2, strlen(string2));
    if (cc < 0) {
        printf(1, "testett: write error\n");
        exit();
    }

    struct stat st;
    fstat(fd2, &st);
    stat2(&st);

    close(fd2);
    exit();
}

