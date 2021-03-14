#include <stdio.h>
#include <stdlib.h>

#include "mem.h"


static void pool_try_shrink(bucket_t *bds, int idx);

static void *pool_alloc_continues(bucket_t *bds, int n)
{
    unsigned long **start, **prev, **block;
    int left = n;

    block = start = bds->free_list;
    prev = (unsigned long **)&bds->free_list;
    while (start && left > 0) {
        if ((unsigned long)start + bds->obj_size == (unsigned long)*start) {
            left--;
        } else {
            prev = start;
            block = (unsigned long **)*start;
            left = n;
        }

        start = (unsigned long **)*start;
    }

    if (left > 0) {
        return NULL;
    }

    /* remove from free_list */
    *prev = (unsigned long *)start;
    bds->free_n -= n;

    return block;
}

static void pool_add_free(bucket_t *bds, int idx, void *addr, int n)
{
    unsigned long **start, **prev;
    unsigned char *block = addr;
    int i = 0;

    if (idx >= BUDY_COUNT)
        return;

    start = bds[idx].free_list;
    prev = (unsigned long **)&bds[idx].free_list;

    while (i < n - 1) {
        *(unsigned long **)(block + i * (OBJ_MIN << idx)) = (unsigned long *)(block + (i + 1) * (OBJ_MIN << idx));
        i++;
    }

    if (!start) {
        *(unsigned long **)(block + (n - 1) * (OBJ_MIN << idx)) = NULL;
        bds[idx].free_list = (unsigned long **)block;
    } else if ((unsigned long)block < (unsigned long)start) {
        *(unsigned long **)(block + (n - 1) * (OBJ_MIN << idx)) = (unsigned long *)bds[idx].free_list;
        bds[idx].free_list = (unsigned long **)block;
    } else {
        while (start && (unsigned long)block > (unsigned long)start) {
            prev = start;
            start = (unsigned long **)*start;
        }
        *(unsigned long **)prev = (unsigned long *)block;
        *(unsigned long ***)(block + (n - 1) * (OBJ_MIN << idx)) = start;
    }

    bds[idx].free_n += n;
    return;
}

static void pool_pull_upper(bucket_t *bds, int idx)
{
    int upper = idx + 1;
    unsigned char *block;

    if (upper >= BUDY_COUNT) {
        return;
    }

    if (!bds[upper].free_n) {
        pool_pull_upper(bds, upper);
    }

    if (!bds[upper].free_n) {
        return;
    }

    block = (unsigned char *)bds[upper].free_list;
    bds[upper].free_list = (unsigned long **)*bds[upper].free_list;
    bds[upper].free_n--;

    /* split into two pieces */
    *(unsigned long **)block = (unsigned long *)(block + (OBJ_MIN << idx));
    *(unsigned long **)(block + (OBJ_MIN << idx)) = NULL; // bds[idx].free_list;

    /* insert into free list sorted by start address */
    pool_add_free(bds, idx, block, 2);
    return;
}

static void pool_try_shrink(bucket_t *bds, int idx)
{
    unsigned long **curr, **prev, **block;

    if (idx >= (BUDY_COUNT - 1)) {
        return;
    }

    curr = bds[idx].free_list;
    prev = (unsigned long **)&bds[idx].free_list;

    while (curr) {
        if (((unsigned long)*curr == (unsigned long)curr + (OBJ_MIN << idx)) &&
            (0 == ((unsigned long)curr & ((OBJ_MIN << idx) - 1)))) {
            /* shrinking */
            block = curr;
            curr = (unsigned long **)*curr;
            curr = (unsigned long **)*curr;
            *prev = (unsigned long *)curr;
            bds[idx].free_n -= 2;
            pool_add_free(bds, idx + 1, block, 1);
            continue; /* keep prev */
        }
        prev = curr;
        curr = (unsigned long **)*curr;
    }

    pool_try_shrink(bds, idx + 1);
    return;
}

bucket_t *pool_create(unsigned char *addr, size_t size)
{
    unsigned long start = (unsigned long)addr;
    bucket_t *budies = (bucket_t *)addr;
    unsigned long **cur;
    unsigned char *p;
    int i, n = 0;

    if (size <= PAGE_SIZE) {
        printf("too low memory\n");
        return NULL;
    }

    start = (start + sizeof(bucket_t) * BUDY_COUNT + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    p = (unsigned char *)start;

    while ((p + PAGE_SIZE) < (addr + size)) {
        cur = (unsigned long **)p;
        *cur = (unsigned long *)(p + PAGE_SIZE);
        p += PAGE_SIZE;
        n++;
    }

    *cur = NULL;

    for (i = 0; i < BUDY_COUNT; i++) {
        budies[i].obj_size = OBJ_MIN << i;
        budies[i].free_n = 0;
        budies[i].free_list = NULL;
    }

    budies[BUDY_COUNT - 1].free_n = n;
    budies[BUDY_COUNT - 1].free_list = (unsigned long **)start;

    return budies;
}

#if 0
static void *pool_alloc_continues(bucket_t *bds, int n)
{
    unsigned long **start, **prev, **block;
    int left = n;

    block = start = bds->free_list;
    prev = &bds->free_list;
    while (start || !left) {
        if ((unsigned long)start == (unsigned long)*start) {
            left--;
        } else {
            prev = start;
            block = (unsigned long **)*start;
            left = n;
        }

        start = (unsigned long **)*start;
    }

    /* remove from free_list */
    if (block) {
        (unsigned long **)*prev = start;
    }

    return block;
}
#endif

void *pool_malloc(bucket_t *bds, size_t size)
{
    int idx = 0;
    size_t tmp = 0;
    block_header_t *bh;

    /* 0 would cause unexpected exception */
    if (!size) {
        return NULL;
    }

    size += sizeof(block_header_t);
    size = (size + MEM_ALIGN - 1) & ~(MEM_ALIGN - 1);

    if (size > bds[BUDY_COUNT - 1].obj_size) {
        int count;

        /* 大块内存, 连续的4K块 */
        size = (size + bds[BUDY_COUNT - 1].obj_size - 1) & ~(bds[BUDY_COUNT -1].obj_size - 1);
        count = size / bds[BUDY_COUNT - 1].obj_size;
        bh = pool_alloc_continues(&bds[BUDY_COUNT - 1], count);
        if (!bh) {
            return NULL;
        }

        bh->idx = BUDY_COUNT - 1;
        bh->count = count;
        bh->magic = OBJ_MAGIC;

        return (bh + 1);
    }

    /* 小块内存 */
    tmp = size;
    while (tmp >>= 1) {
        idx++;
    }

    tmp = ((1 << idx) - 1 + size) & ~((1 << idx) - 1);
    if (tmp == size) {
        idx -= 1;
    }
    idx -= 3; /* 调整到索引*/
    if (!bds[idx].free_n) {
        pool_pull_upper(bds, idx);
    }

    if (!bds[idx].free_n) {
        /* no memory */
        return NULL;
    }

    bh = (block_header_t *)bds[idx].free_list;
    bds[idx].free_list = (unsigned long **)*bds[idx].free_list;
    bds[idx].free_n -= 1;

    bh->idx = idx;
    bh->count = 1;
    bh->magic = OBJ_MAGIC;

    return (bh + 1);
}

void pool_free(bucket_t *bds, void *addr)
{
    block_header_t *bh = addr;
    int idx = 0;

    bh--;   /* offset to  header */
    if (bh->magic != OBJ_MAGIC) {
        printf("something wrong ...\n");
        return;
    }

    if (bh->idx >= BUDY_COUNT) {
        printf("invalid idx ...\n");
        return;
    }

    idx = bh->idx;  /* will be overwrite, save it */
    pool_add_free(bds, bh->idx, bh, bh->count);

    if (bds[idx].free_n > SHRINK_THD) {
        pool_try_shrink(bds, idx);
    }

    return;
}

void pool_info(bucket_t *bds)
{
    int i;

    for (i = 0; i < BUDY_COUNT; i++) {
        printf("budy index %d, budy_size %d, free count %d, free list %p\n", i, bds[i].obj_size, bds[i].free_n, bds[i].free_list);
    }
}

