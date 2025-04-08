// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU];

static char locknames[16 * NCPU];

static char *lockname(int id)
{
  char buf[16];
  uint64 i = 0;
  char *name = &locknames[16 * id];

  do
  {
    buf[i++] = id % 10 + '0';
  } while ((id /= 10) > 0);

  memmove(name, "kmem_", sizeof("kmem_") - 1);

  for (uint64 j = 0; j < i; ++j)
  {
    *(name + j + sizeof("kmem_") - 1) = buf[i - 1 - j];
  }

  name[i + sizeof("kmem_") - 1] = '\0';

  return name;
}

void
kinit()
{
  for (int id = 0; id < NCPU; id++)
  {
    char *name = lockname(id);
    initlock(&kmem[id].lock, name);
  }

  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  int cpu;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  // cpuid() is safe only when interrupts are turned off
  push_off();
  cpu = cpuid();
  acquire(&kmem[cpu].lock);

  r->next = kmem[cpu].freelist;
  kmem[cpu].freelist = r;

  release(&kmem[cpu].lock);
  pop_off();
}

static
struct run *
  kallocfrom(int cpu)
{
  struct run *r;

  acquire(&kmem[cpu].lock);

  r = kmem[cpu].freelist;

  if (r)
    kmem[cpu].freelist = r->next;

  release(&kmem[cpu].lock);

  return r;
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  push_off();
  r = kallocfrom(cpuid());
  pop_off();

  // steal from others if no free memory on this cpu
  for (int i = 0; !r && i < NCPU; ++i)
  {
    r = kallocfrom(i);
  }

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk

  return (void*)r;
}
