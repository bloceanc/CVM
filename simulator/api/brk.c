#include <stdio.h>
#include <stdlib.h>

#include "../base.h"
#include "../cpu.h"
#include "../vm.h"
#include "api.h"


/**
 * int brk(void *addr)
 */
s32_t cpu0_brk(vm_t *vm, u32_t tid, ...)
{
    u32_t addr = abi_func_param_n(vm, tid, 1);

    if ((vm->mem.start + addr) >= vm->mem.end) {
        printf("brk failed, no memory!\n");
        abi_func_set_return(vm, -1);
    } else {
        vm->mem.brk = (void *)(unsigned long)addr;
        abi_func_set_return(vm, 0);
    }
    return 0;
}

/**
 * void *sbrk(u32_t increment)
 */
s32_t cpu0_sbrk(vm_t *vm, u32_t tid, ...)
{
    u32_t incr = abi_func_param_n(vm, tid, 1);
    u32_t addr;

    if ((vm->mem.end - vm->mem.brk) <= incr) {
        printf("sbrk failed, no memory!\n");
        abi_func_set_return(vm, -1);
        return 0;
    }

    addr = (u32_t)(unsigned long)vm->mem.brk;
    vm->mem.brk += incr;
    abi_func_set_return(vm, addr);
    return 0;
}

