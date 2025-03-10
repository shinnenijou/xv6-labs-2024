#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define STDIN_FD 0
#define MAXLINE 4096

static int readline(int fd, char *buf, int maxlen)
{
    int len = 0;

    for (; len < maxlen; ++len)
    {
        int rc = read(fd, buf + len, 1);

        if (rc == 0) /* EOF */
        {
            break;
        }
        else if (rc < 0) /* system call error */
        {
            return -1;
        }

        if (buf[len] == '\n') /* new line */
        {
            break;
        }
    }

    if (len < maxlen)
    {
        buf[len] = '\0';
    }

    return len;
}

/* may modify user buf. return past-to-end index */
static int splitline(char *buf, char **tokens, int start, int end)
{
    int token = start;
    char *startp = 0;
    uint len = strlen(buf);

    for (uint i = 0; i < len && token < end; ++i)
    {
        if (buf[i] != ' ' && !startp)
        {
            startp = buf + i;
        }

        if (buf[i] == ' ' && startp)
        {
            buf[i] = '\0';
            tokens[token++] = startp;
            startp = 0;
        }
    }

    if (startp && token < end)
    {
        tokens[token++] = startp;
    }

    return token;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("usage: xargs command [arguments]");
        exit(1);
    }

    char *cmd = argv[1];
    char *child_argv[MAXARG + 1];
    int child_argc = 0;

    for (int i = 1; i < argc; ++i)
    {
        child_argv[child_argc++] = argv[i];
    }

    child_argv[child_argc] = 0;

    char buf[1024];
    int n = 0;

    while ((n = readline(STDIN_FD, buf, sizeof(buf))) > 0)
    {
        if (n == sizeof(buf))
        {
            printf("too long arguments\n");
            continue;
        }

        int null_pos = splitline(buf, child_argv, child_argc, MAXARG + 1);

        if (null_pos > MAXARG)
        {
            printf("too many arguments\n");
            continue;
        }

        child_argv[null_pos] = 0;

        if (fork() == 0) /* child */
        {
            if (exec(cmd, child_argv) < 0)
            {
                exit(1);
            }
        }

        /* parent */
        while (wait(0) > 0)
            ;
    }

    exit(0);
}