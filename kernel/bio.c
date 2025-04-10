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
  struct spinlock bucketlocks[HASH_BUCKETS_NUM];

  // hash buckets for buffers
  // the last bucket contains unused buffer, and the other contain used buffer
  struct buf buckets[HASH_BUCKETS_NUM];
} bcache;

void
binit(void)
{
  initlock(&bcache.lock, "bcache");

  // init buckets
  for (int i = 0; i < HASH_BUCKETS_NUM; i++)
  {
    initlock(&bcache.bucketlocks[i], "bcache");
    bcache.buckets[i].next = &bcache.buckets[i];
    bcache.buckets[i].prev = &bcache.buckets[i];
  }

  // init unused buffers
  for (int i = 0; i < NBUF; i++)
  {
    struct buf *b = &bcache.buf[i];
    initsleeplock(&b->lock, "buffer");
    b->next = bcache.buckets[0].next;
    b->prev = &bcache.buckets[0];
    bcache.buckets[0].next->prev = b;
    bcache.buckets[0].next = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  // Is the block already cached?
  uint hash = blockno % HASH_BUCKETS_NUM;
  acquire(&bcache.bucketlocks[hash]);

  for (b = bcache.buckets[hash].next; b != &bcache.buckets[hash]; b = b->next)
  {
    if (b->dev == dev && b->blockno == blockno)
    {
      b->refcnt++;
      release(&bcache.bucketlocks[hash]);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // No cached
  // try to recycle from current bucket
  for (b = bcache.buckets[hash].next; b != &bcache.buckets[hash]; b = b->next)
  {
    if (b->refcnt == 0)
    {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.bucketlocks[hash]);
      acquiresleep(&b->lock);
      return b;
    }
  }

  release(&bcache.bucketlocks[hash]);

  // recycle from other buckets
  // search from the next bucket to avoid potential contend
  for (uint i = (hash + 1) % HASH_BUCKETS_NUM; i != hash; i = (i + 1) % HASH_BUCKETS_NUM)
  {
    acquire(&bcache.bucketlocks[i]);

    for (b = bcache.buckets[i].next; b != &bcache.buckets[i]; b = b->next)
    {
      if (b->refcnt == 0)
      {
        b->dev = dev;
        b->blockno = blockno;
        b->valid = 0;
        b->refcnt = 1;

        // remove from current bucket
        b->next->prev = b->prev;
        b->prev->next = b->next;
        release(&bcache.bucketlocks[i]);

        // add into correct bucket
        acquire(&bcache.bucketlocks[hash]);
        b->next = bcache.buckets[hash].next;
        b->prev = &bcache.buckets[hash];
        bcache.buckets[hash].next->prev = b;
        bcache.buckets[hash].next = b;
        release(&bcache.bucketlocks[hash]);

        acquiresleep(&b->lock);
        return b;
      }
    }

    release(&bcache.bucketlocks[i]);
  }

  panic("bget: no buffers");
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

  uint hash = b->blockno % HASH_BUCKETS_NUM;
  acquire(&bcache.bucketlocks[hash]);
  b->refcnt--;
  release(&bcache.bucketlocks[hash]);
}

void
bpin(struct buf *b) {
  uint hash = b->blockno % HASH_BUCKETS_NUM;
  acquire(&bcache.bucketlocks[hash]);
  b->refcnt++;
  release(&bcache.bucketlocks[hash]);
}

void
bunpin(struct buf *b) {
  uint hash = b->blockno % HASH_BUCKETS_NUM;
  acquire(&bcache.bucketlocks[hash]);
  b->refcnt--;
  release(&bcache.bucketlocks[hash]);
}


