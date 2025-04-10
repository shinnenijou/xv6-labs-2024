// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define HASH_BUCKETS_NUM 17

struct {
  struct spinlock lock;
  struct buf buf[NBUF];

  // bucket locks
  // one lock for each bucket
  struct spinlock locks[HASH_BUCKETS_NUM + 1];

  // hash buckets for buffers
  // the last bucket contains unused buffer, and the other contain used buffer
  struct buf buckets[HASH_BUCKETS_NUM + 1];
} bcache;

void
binit(void)
{
  initlock(&bcache.lock, "bcache");

  // init buckets
  for (int i = 0; i <= HASH_BUCKETS_NUM; i++)
  {
    initlock(&bcache.locks[i], "bcache");
    bcache.buckets[i].next = &bcache.buckets[i];
    bcache.buckets[i].prev = &bcache.buckets[i];
  }

  // init unused buffers
  for (int i = 0; i < NBUF; i++)
  {
    struct buf *b = &bcache.buf[i];
    initsleeplock(&b->lock, "buffer");
    b->next = bcache.buckets[HASH_BUCKETS_NUM].next;
    b->prev = &bcache.buckets[HASH_BUCKETS_NUM];
    bcache.buckets[HASH_BUCKETS_NUM].next->prev = b;
    bcache.buckets[HASH_BUCKETS_NUM].next = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  acquire(&bcache.lock);

  // Is the block already cached?
  uint hash = blockno % HASH_BUCKETS_NUM;
  acquire(&bcache.locks[hash]);

  for (b = bcache.buckets[hash].next; b != &bcache.buckets[hash]; b = b->next)
  {
    if (b->dev == dev && b->blockno == blockno)
    {
      b->refcnt++;
      release(&bcache.locks[hash]);
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }

  release(&bcache.locks[hash]);

  // Not cached.
  // recycle from unused buffers
  acquire(&bcache.locks[HASH_BUCKETS_NUM]);
  b = bcache.buckets[HASH_BUCKETS_NUM].next;

  if (b == &bcache.buckets[HASH_BUCKETS_NUM])
  {
    panic("bget: no buffers");
  }

  b->dev = dev;
  b->blockno = blockno;
  b->valid = 0;
  b->refcnt = 1;

  // remove from last bucket
  bcache.buckets[HASH_BUCKETS_NUM].next = b->next;
  bcache.buckets[HASH_BUCKETS_NUM].next->prev = &bcache.buckets[HASH_BUCKETS_NUM];
  release(&bcache.locks[HASH_BUCKETS_NUM]);

  // add into hashed bucket
  acquire(&bcache.locks[hash]);
  b->next = bcache.buckets[hash].next;
  b->prev = &bcache.buckets[hash];
  bcache.buckets[hash].next->prev = b;
  bcache.buckets[hash].next = b;
  release(&bcache.locks[hash]);

  release(&bcache.lock);
  acquiresleep(&b->lock);
  return b;
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.lock);
  b->refcnt--;
  if (b->refcnt == 0) {
    // no one is waiting for it.
    uint hash = b->blockno % HASH_BUCKETS_NUM;

    // remove from current bucket
    acquire(&bcache.locks[hash]);
    b->next->prev = b->prev;
    b->prev->next = b->next;
    release(&bcache.locks[hash]);

    // add into unused buffer bucket
    acquire(&bcache.locks[HASH_BUCKETS_NUM]);
    b->next = bcache.buckets[HASH_BUCKETS_NUM].next;
    b->prev = &bcache.buckets[HASH_BUCKETS_NUM];
    bcache.buckets[HASH_BUCKETS_NUM].next->prev = b;
    bcache.buckets[HASH_BUCKETS_NUM].next = b;
    release(&bcache.locks[HASH_BUCKETS_NUM]);
  }
  
  release(&bcache.lock);
}

void
bpin(struct buf *b) {
  uint hash = b->blockno % HASH_BUCKETS_NUM;
  acquire(&bcache.lock);
  acquire(&bcache.locks[hash]);
  b->refcnt++;
  release(&bcache.locks[hash]);
  release(&bcache.lock);
}

void
bunpin(struct buf *b) {
  uint hash = b->blockno % HASH_BUCKETS_NUM;
  acquire(&bcache.lock);
  acquire(&bcache.locks[hash]);
  b->refcnt--;
  release(&bcache.locks[hash]);
  release(&bcache.lock);
}


