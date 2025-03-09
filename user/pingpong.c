#include "kernel/types.h"
#include "user/user.h"

#ifndef STDOUT_FD
#define STDOUT_FD 1
#endif

int main(int argc, char *argv[])
{
    int p[2];
    if (pipe(p) < 0)
    {
        exit(1);
    }

    if (fork() == 0) /* child */
    {
        char buffer[1];

        read(p[0], buffer, 1);
        printf("%d: received ping\n", getpid());
        write(p[1], buffer, 1);
        close(p[0]);
        close(p[1]);

        exit(0);
    }

    /* parent */
    char buffer[1] = {'!'};
    write(p[1], buffer, 1);

    /* parent will read the same pipe file with child thus need to wait for child exit */
    wait(0);

    read(p[0], buffer, 1);
    printf("%d: received pong\n", getpid());
    close(p[0]);
    close(p[1]);

    exit(0);
}