#include "kernel/types.h"
#include "user/user.h"

#define STDOUT_FD 1
#define PIPE_RD 0
#define PIPE_WR 1
#define NUM_START 2
#define NUM_END 280

static int prime(int rdfd, int wrfd) __attribute__((noreturn));

static int prime(int rdfd, int wrfd)
{
    int pipefd[2];
    int p = 0;
    int num = 0;
    int pid = 0;

    if (read(rdfd, &p, sizeof(int)) < 0)
    {
        exit(1);
    }

    printf("prime %d\n", p);

    while (read(rdfd, &num, sizeof(int)) > 0)
    {
        if (num % p == 0)
        {
            continue;
        }

        if (pid == 0) /* no child yet */
        {
            pipe(pipefd);

            if ((pid = fork()) == 0) /* child */
            {
                close(rdfd);
                close(pipefd[PIPE_WR]);
                prime(pipefd[PIPE_RD], -1); /* will exit in prime() function */
            }

            wrfd = pipefd[PIPE_WR];
            close(pipefd[PIPE_RD]);
        }

        write(wrfd, &num, sizeof(int));
    }

    close(rdfd);

    if (wrfd > 0)
    {
        close(wrfd);
    }

    while (wait(0) > 0)
    {
    }

    exit(0);
}

int main(int argc, char *argv[])
{
    int pipefd[2] = {0, 0};

    if (pipe(pipefd) < 0)
    {
        exit(1);
    }

    if (fork() == 0) /* child */
    {
        close(pipefd[PIPE_WR]);
        prime(pipefd[PIPE_RD], -1); /* exit in prime routine */
    }

    /* parent */
    close(pipefd[PIPE_RD]);

    for (int i = NUM_START; i < NUM_END + 1; ++i)
    {
        write(pipefd[PIPE_WR], &i, sizeof(int));
    }

    close(pipefd[PIPE_WR]);

    while (wait(0) < 0)
    {
    }

    exit(0);
}