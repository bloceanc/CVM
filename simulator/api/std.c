#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../base.h"
#include "../cpu.h"
#include "../vm.h"
#include "api.h"


/**
 * uint32 time(void)
 */
s32_t cpu0_time(vm_t *vm, u32_t tid, ...)
{
    unsigned long timestamp = time(NULL);
    abi_func_set_return(vm, timestamp & 0xFFFFFFFF);
    return 0;
}

