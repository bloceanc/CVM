#include <stdio.h>
#include <stdlib.h>

#include "../base.h"
#include "../cpu.h"
#include "../vm.h"
#include "api.h"

extern
s32_t cpu0_brk(vm_t *vm, u32_t tid, ...);
extern
s32_t cpu0_sbrk(vm_t *vm, u32_t tid, ...);
extern
s32_t cpu0_printf(vm_t *vm, u32_t tid, ...);
extern
s32_t cpu0_open(vm_t *vm, u32_t tid, ...);
extern
s32_t cpu0_read(vm_t *vm, u32_t tid, ...);
extern
s32_t cpu0_write(vm_t *vm, u32_t tid, ...);
extern
s32_t cpu0_close(vm_t *vm, u32_t tid, ...);
extern
s32_t cpu0_lseek(vm_t *vm, u32_t tid, ...);
extern
s32_t cpu0_time(vm_t *vm, u32_t tid, ...);

API_CALL_PF api_func[] = {
    [API_ID_MIN]      = NULL,
    [API_ID_PRINTF]   = cpu0_printf,
    [API_ID_BRK]      = cpu0_brk,
    [API_ID_SBRK]     = cpu0_sbrk,
    [API_ID_OPEN]     = cpu0_open,
    [API_ID_READ]     = cpu0_read,
    [API_ID_WRITE]    = cpu0_write,
    [API_ID_CLOSE]    = cpu0_close,
    [API_ID_LSEEK]    = cpu0_lseek,
    [API_ID_TIME]     = cpu0_time,
    [API_ID_MAX]      = NULL,
};

