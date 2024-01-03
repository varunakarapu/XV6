#include "types.h"
#include "user.h"
#include "fs.h"
#include "define.h"
#define NUM_BLOCKS 16523
#define BLOCK_SIZE BSIZE
#define CHUNK_SIZE 1024

int main() {
    int fd = open("testbig.file", O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf(1, "testbig: cannot open testbig.file for writing\n");
        exit();
    }

    char data[CHUNK_SIZE];
    for (int i = 0; i < NUM_BLOCKS; i++) {
        *(int*)data = i;

        int remaining = BLOCK_SIZE;
        while (remaining > 0) {
            int chunk_size = CHUNK_SIZE;
            if (remaining < CHUNK_SIZE) {
                chunk_size = remaining;
            }

            int cc = write(fd, data, chunk_size);
            if (cc < 0) {
                printf(1, "testbig: write error at block %d\n", i);
                exit();
            }

            remaining -= cc;
        }

        if ((i % 1000 == 0)|| (i==NUM_BLOCKS-1)) {
            printf(1, "testbig: completed writing block %d\n", i+1);
        }
    }

    close(fd);

    fd = open("testbig.file", O_RDONLY);
    if (fd < 0) {
        printf(1, "testbig: cannot re-open testbig.file for reading\n");
        exit();
    }

    char buffer[BLOCK_SIZE];
    for (int i = 0; i < NUM_BLOCKS; i++) {
        int remaining = BLOCK_SIZE;
        char *pos = buffer;

        while (remaining > 0) {
            int chunk_size = CHUNK_SIZE;
            if (remaining < CHUNK_SIZE) {
                chunk_size = remaining;
            }

            int cc = read(fd, pos, chunk_size);
            if (cc < 0) {
                printf(1, "testbig: read error at block %d\n", i);
                exit();
            }
            remaining -= cc;
            pos += cc;
        }

        if (*(int*)buffer != i) {
            printf(1, "testbig: read the wrong data (%d) for block %d\n", *(int*)buffer, i);
            exit();
        }

        if ((i % 1000 == 0) || (i==NUM_BLOCKS-1)) {
            printf(1, "testbig: completed reading block %d\n", i+1);
        }
    }

    printf(1, "testbig done; ok\n");
    exit();
}

