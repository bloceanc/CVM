#ifndef _CPU0_SYSCALL_H_
#define _CPU0_SYSCALL_H_

typedef s32_t (*API_CALL_PF)(vm_t *vm, u32_t tid, ...);

enum api_e {
    API_ID_MIN      = 0x0,
    API_ID_PRINTF   = 0x1,
    API_ID_BRK      = 0x2,
    API_ID_SBRK     = 0x3,
    API_ID_OPEN     = 0x4,
    API_ID_READ     = 0x5,
    API_ID_WRITE    = 0x6,
    API_ID_CLOSE    = 0x7,
    API_ID_LSEEK    = 0x8,
    API_ID_TIME     = 0x9,
    API_ID_MAX      = 0xa,
};

extern API_CALL_PF  api_func[];

#endif /* _CPU0_SYSCALL_H_ */

