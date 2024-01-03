#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
    printf(1, "Pid: %d\n", getpid());
    set_sched_priority(3);
    printf(1, "Priority set to 3\n");
    printf(1, "Get: %d\n", get_sched_priority(getpid()));
    int i = 0;
    for (i = 0; i < 20; i++) {
      printf(1, "Ticks: %d\n", time_scheduled(getpid()));
    }
    exit();
}
