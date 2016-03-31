/*
 * fast_slob.h: a simple but fast linked-list based buffer allocator
 * Copyright (c) 2012 Rong Shen <rong1129@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef _FAST_SLOB_H
#define _FAST_SLOB_H

#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/cache.h>

#define PRINT(value) printk("%s:%d\n",#value,value)

#define MIN_ALLOC_SIZE		sizeof(char *)
#define printf printk
void hexdump(const void *buf, unsigned long size)
{
  int col = 0, off = 0;
  unsigned char *p = (unsigned char *)buf;

  while (size--) {
    if (!col)
      printf("\t%08x:", off);

    printf(" %02x", *p++);

    off++;
    col++;

    if (!(col % 16)) {
      printf("\n");
      col = 0;
    } else if (!(col % 4))
      printf("  ");
  }

  if (col % 16)
    printf("\n\n");
  else
    printf("\n");
}

struct fast_slob {
	spinlock_t lock;

	size_t bucket_size;
	int min_alloc_size;
	int max_alloc_size;
	int alloc_size_shift;
	int num_buckets;

	char *start, *end;

	char *buckets[0];
};


static inline struct fast_slob *fast_slob_create(size_t size, int max_alloc_size, int alloc_size_shift, int num_buckets)
{
	struct fast_slob *slob;
	size_t bucket_size, min_alloc_size;
	char *start, *end, *buf;
	int i;

	if (max_alloc_size < MIN_ALLOC_SIZE || alloc_size_shift < 1 || num_buckets < 1)
		return NULL;

	bucket_size = L1_CACHE_ALIGN(size / num_buckets);
    printk("%d,%d,%d\n",bucket_size,MIN_ALLOC_SIZE,max_alloc_size);
	if (bucket_size * num_buckets > size)
		bucket_size = (size / num_buckets) & ~(L1_CACHE_BYTES - 1);
	if (bucket_size < max_alloc_size)
		return NULL;

	min_alloc_size = max_alloc_size >> (alloc_size_shift * (num_buckets - 1));
	if (min_alloc_size < MIN_ALLOC_SIZE)
		return NULL;
    
    printk("%d,%d,%d,%d,%d\n",bucket_size,min_alloc_size,MIN_ALLOC_SIZE,max_alloc_size,num_buckets);
      
	slob = kmalloc(sizeof(*slob) + num_buckets * sizeof(char *), GFP_KERNEL);
	if (!slob)
		return NULL;
  
	slob->start = vmalloc_user(size);
	if (!slob->start) {
		kfree(slob);
		return NULL;
	}

	slob->end = slob->start + size;
	slob->bucket_size = bucket_size;
	slob->min_alloc_size = min_alloc_size;
	slob->max_alloc_size = max_alloc_size;
	slob->alloc_size_shift = alloc_size_shift;
	slob->num_buckets = num_buckets;

	for (i = 0; i < num_buckets; i++) {
		start = slob->start + i * bucket_size;
		end = start + bucket_size;
		slob->buckets[i] = start;
        printk("%p,%p\n",start,end);

		while (start < end) {
			buf = start;
            printk("start=0x%p,*start=0x%x\n",start,*(unsigned long*)start);
			start += ALIGN(min_alloc_size, MIN_ALLOC_SIZE);
			//*(unsigned long*)buf = (start < end) ? start : NULL;
			*(char ***)buf = (start < end) ? start : NULL;
			//*(char **)buf = (start < end) ? start : NULL;
            //printk("start=%p,%x--->%x\n",start,(unsigned long*)buf,*(unsigned long*)buf);
            printk("start=0x%p,0x%x--->0x%x\n",start,buf,*(unsigned long*)buf);
            //printk("start=%p,*start=%x\n",start,*(unsigned long*)buf);
		}

		min_alloc_size <<= alloc_size_shift;
	}
    //hexdump(slob->start,size);

	spin_lock_init(&slob->lock);
	return slob;
}

static inline void fast_slob_destroy(struct fast_slob *slob)
{
	vfree(slob->start);
	kfree(slob);
}

static inline void *fast_slob_alloc(struct fast_slob *slob, size_t size)
{
	size_t alloc_size = slob->min_alloc_size;
	char *p;
	int i;

	spin_lock(&slob->lock);
	for (i = 0; i < slob->num_buckets; i++) {
		if (alloc_size >= size && slob->buckets[i]) {
			p = slob->buckets[i];
			slob->buckets[i] = *(char **)p;
			spin_unlock(&slob->lock);
			return p;
		}
		alloc_size <<= slob->alloc_size_shift;
	}
	spin_unlock(&slob->lock);

	return NULL;
}

static inline int fast_slob_bucket(struct fast_slob *slob, void *p)
{
	size_t off, alloc_size;
	int idx;

	if ((char *)p < slob->start || (char *)p >= slob->end)
		return -1;

	off = (char *)p - slob->start;
	idx = off / slob->bucket_size;
	alloc_size = slob->min_alloc_size << (idx * slob->alloc_size_shift);
	if ((off - idx * slob->bucket_size) % ALIGN(alloc_size, MIN_ALLOC_SIZE))
		return -1;

	return idx;
}

static inline void _fast_slob_free(struct fast_slob *slob, int idx, void *p)
{
	spin_lock(&slob->lock);
	*(char **)p = slob->buckets[idx];
	slob->buckets[idx] = p;
	spin_unlock(&slob->lock);
}

static inline void fast_slob_free(struct fast_slob *slob, void *p)
{
	int idx;

	if ((idx = fast_slob_bucket(slob, p)) < 0) {
		printk(KERN_WARNING "fast_slob: try to free an invalid buffer with address %p\n", p);
		return;
	}

	_fast_slob_free(slob, idx, p);
}

#endif	/* _FAST_SLOB_H */
