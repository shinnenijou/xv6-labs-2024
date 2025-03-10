#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

#define FS_SEP '/'

static char *retrieve_filename(char *path)
{
    int i = (int)strlen(path);

    /* find the first character after last slash. */
    while (i >= 0 && path[i] != FS_SEP)
        --i;

    return path + i + 1;
}

static void find(char *path, char *keyword)
{
    int fd = open(path, O_RDONLY);
    struct stat st;

    if (fd < 0)
    {
        printf("find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        printf("find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_DEVICE: /* print device same as file */
    case T_FILE:
    {
        if (strcmp(retrieve_filename(path), keyword) == 0)
        {
            printf("%s\n", path);
        }
    }
    break;
    case T_DIR:
    {
        char buf[512];
        uint len = strlen(path);

        if (len + 1 + DIRSIZ + 1 > sizeof(buf))
        {
            printf("ls: path too long\n");
            break;
        }

        memcpy(buf, path, len);
        buf[len++] = FS_SEP;
        struct dirent de;

        while (read(fd, &de, sizeof(de)) > 0)
        {
            if (de.inum == 0)
            {
                continue;
            }

            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            {
                continue;
            }

            memcpy(buf + len, de.name, DIRSIZ);
            buf[len + DIRSIZ] = '\0';
            find(buf, keyword);
        }
    }
    break;
    default:
        break;
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("usage: find path ... [expression]\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}