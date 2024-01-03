#include "types.h"
#include "user.h"

int 
main(int argc, char* argv[]) {
    if (argc < 2) {
        printf(1, "usage: sleep <+ve integer>\n");
        exit();
    }
    // convert string to number
    int dur = atoi(argv[1]);
    // using the sleep system call
    sleep(dur*50);

    exit();
}
