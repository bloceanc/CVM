#include <stdio.h>
#include <stdlib.h>

#include "../base.h"
#include "../cpu.h"
#include "../vm.h"
#include "api.h"

#define MAX_CACHE_BUFF_SIZE 4096

/**
 * printf(s8_t *fmt, ...)
 */
s32_t cpu0_printf(vm_t *vm, u32_t tid, ...)
{
    s8_t *fmt = NULL;
    s8_t buf[MAX_CACHE_BUFF_SIZE];
    s8_t *p = NULL, t = 0;
    s32_t n = 0, param_idx = 0, arg;

    arg = abi_func_param_n(vm, tid, ++param_idx);
    fmt = (s8_t *)ABI_PARAM_TO_ADDR(vm, arg);
    p = fmt;

    while (n < (s32_t)(sizeof(buf) && *p)) {
        if (*p == '%') {
            t = *(++p);
            arg = abi_func_param_n(vm, tid, ++param_idx);
            switch (t) {
                case 'd':
                    n += snprintf(&buf[n], sizeof(buf) - n, "%d", arg);
                    break;
                case 'x':
                    n += snprintf(&buf[n], sizeof(buf) - n, "%x", arg);
                    break;
                case 'X':
                    n += snprintf(&buf[n], sizeof(buf) - n, "%X", arg);
                    break;
                case 's':
                    n += snprintf(&buf[n], sizeof(buf) - n, "%s", (s8_t *)(ABI_PARAM_TO_ADDR(vm, arg)));
                    break;
                case 'c':
                    n += snprintf(&buf[n], sizeof(buf) - n, "%c", arg);
                    break;
                default:
                    printf("not supported printf format %c\n", t);
                    break;
            }
        } else {
            buf[n] = *p;
            n++;
        }
        p++;
    }

    if (n == sizeof(buf)) {
        buf[n-1] = 0;
    } else {
        buf[n-1] = 0;
    }

    abi_func_set_return(vm, puts(buf));

    return 0;
}

