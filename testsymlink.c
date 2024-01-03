#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "define.h"
int main(void) 
{
    char *oldpath = "file.txt";
    char *newpath = "link.txt";
    char buf[128];
    int fd;

    // Create a test file
    if ((fd = open(oldpath, O_CREATE | O_WRONLY)) < 0)
    {
        printf(1, "error creating file\n");
        exit();
    
    }
    char contents[12] = "1234567890";
    printf(1,"Writing the contents : %s to old path \n",contents);
    write(fd, contents , 10);
    close(fd);

    // Create a symbolic link
    int symlinkValue = symlink(oldpath, newpath);
    printf(1,"Checking if symlink value is either positive or zero, indicating it is created\n");
    printf(1,"symlink value: %d\n",symlinkValue);
    if (symlinkValue < 0)
    {
        printf(1, "error creating symbolic link\n");
        exit();
    }
    printf(1,"Symbolic Link created \n");

    // Read from the symbolic link
    if ((fd = open(newpath, O_RDONLY)) < 0)
    {
        printf(1, "error opening symbolic link\n");
        exit();
    }
    int n = read(fd, buf, sizeof(buf));
    printf(1, "read %d bytes from newPath before unlinking: %s\n", n, buf);
    close(fd);

    // Remove the symbolic link
    if (unlink(newpath) < 0)
    {
        printf(1, "error removing symbolic link\n");
        exit();
    }
    printf(1,"New path unlinked \n");
   

    exit();
}

