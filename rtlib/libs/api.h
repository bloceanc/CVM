#ifndef _RT_API_H_
#define _RT_API_H_



#define API_MIN     0
#define API_PRINTF  1
#define API_BRK     2
#define API_SBRK    3
#define API_OPEN    4
#define API_READ    5
#define API_WRITE   6
#define API_CLOSE   7
#define API_LSEEK   8
#define API_TIME    9


/* brk */
extern int brk(void *addr);
extern void *sbrk(long increment);


/* file */
#define F_O_RDONLY  1
#define F_O_WRONLY  2
#define F_O_RDWR    3
#define F_O_CREAT   4

#define F_SEEK_SET  0
#define F_SEEK_END  1
#define F_SEEK_CUR  2

extern int open(const char *filename, int flags);
extern int read(int fd, char *buf, int size);
extern int write(int fd, char *buf, int size);
extern int close(int fd);
extern int lseek(int fd, int offset, int whence);


/* memory */
#define NULL 0

extern void mem_init(int size);
extern void *malloc(int size);
extern void free(void *addr);


/*------------------------- inline -------------------*/
static inline void memcpy(unsigned char *dst, unsigned char *src, int len)
{
    while (len--) {
        *(dst++) = *(src++);
    }
}

static inline void memset(unsigned char *dst, unsigned char v, int len)
{
    int i;
    
    for (i = 0; i < len; i++) {
        dst[i] = v;
    }
}

#endif /* _RT_API_H_ */
