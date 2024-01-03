#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "define.h"
int main(void) 
{
    int fd;
    char buf[128];

    // Create a test file
    if ((fd = open("testfile", O_CREATE | O_WRONLY)) < 0)
    {
        printf(1, "error creating file\n");
        exit();
    }
    char contents[12]= "1234567890";
    printf(1, "Writing lseek to a file with contents: %s \n",contents);
    write(fd, contents, 10);
    close(fd);

    // Test lseek
    if ((fd = open("testfile", O_RDONLY)) < 0)
    {
        printf(1, "error opening file\n");
        exit();
    }
    int offset = 3;
    printf(1,"offset value: %d \n",offset);
    if (lseek(fd, offset, SEEK_SET) < 0)
    {
        printf(1, "error seeking file\n");
        exit();
    }

    int n = read(fd, buf, sizeof(buf));
    printf(1, "read remaining %d bytes excluding the offset from file: %s\n", n, buf);
    printf(1, "lseek executed successfully\n");
    close(fd);
    exit();
}

