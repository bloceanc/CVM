#ifndef _MEM_ALLOC_H_
#define _MEM_ALLOC_H_

#define PAGE_SIZE   4096
#define BUDY_COUNT  9   /* do not change */
#define MEM_ALIGN   8
#define SHRINK_THD  16
#define OBJ_MIN     16  /* do not change */
#define OBJ_MAGIC   0xA1B2C3D4
#define CHUNCK_SIZE PAGE_SIZE

typedef struct bucket_s {
    unsigned int   obj_size; /* 对象大小 */
    unsigned int   free_n;    /* 空闲对象数量 */
    unsigned long **free_list;
} bucket_t;

typedef struct block_header_s {
    unsigned short idx;
    unsigned short count;
    unsigned int magic;
} block_header_t;

extern void poll_info(bucket_t *bds);

#endif /* _MEM_ALLOC_H_ */

