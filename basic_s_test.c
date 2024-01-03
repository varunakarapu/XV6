#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
    printf(1, "Pid: %d\n", getpid());
    int i = 0;
    printf(1, "Pos: %d\n", rr_position(getpid()));

    for (i = 0; i < 20; i++) {
      printf(1, "Ticks: %d\n", time_scheduled(getpid()));
    }
    exit();
}
