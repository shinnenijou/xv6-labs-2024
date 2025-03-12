#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define SECRET_LEN 7
#define PAGE_OFFSET 32

static int in_secret_set(char c){
  return c == '.' || c == '/' || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f';
}

int main(int argc, char *argv[])
{
  // your code here.  you should write the secret to fd 2 using write
  // (e.g., write(2, secret, 8)

  int try_page = 32;
  char *start = sbrk(PGSIZE * try_page);

  for (char *p = start; p < start + PGSIZE * try_page; p += PGSIZE){
    int hit = 1;

    for (int i = PAGE_OFFSET; i < PAGE_OFFSET + SECRET_LEN; ++i){
      if (!in_secret_set(p[i])){
        hit = 0;
        break;
      }
    }

    hit = hit && p[PAGE_OFFSET + SECRET_LEN] == '\0';

    if (hit)
    {
      write(2, p + PAGE_OFFSET, SECRET_LEN + 1);
      break;
    }
  }

  exit(1);
}
