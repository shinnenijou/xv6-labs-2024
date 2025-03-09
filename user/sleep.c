#include "kernel/types.h"
#include "user/user.h"

#ifndef STDOUT_FD
#define STDOUT_FD 1
#endif

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        write(STDOUT_FD, "Usage: sleep NUMBERS\n", 21);
        exit(1);
    }

    int ticks = atoi(argv[1]);

    if (sleep(ticks) < 0)
    {
        exit(1);
    }

    exit(0);
}