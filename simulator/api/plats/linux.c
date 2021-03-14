#include <stdio.h>
#include <stdlib.h>

#include "../base.h"
#include "../cpu.h"
#include "../lib.h"

#define MAX_CACHE_BUFF_SIZE 4096

s32_t printf(s8_t *fmt, ...)
{
    s8_t buf[MAX_CACHE_BUFF_SIZE];
    s8_t *p = fmt, *c = buf, t = 0;
    s32_t n, param_idx = 1;

    while (1) {
        if (*p == '%') {
            t = *(++p);
            switch (t) {
                case 'd':
                    
            }
        } else {
            *c = *p;
            c++;
        }
        p++;
    }

    n = snprintf(fmt, sizeof(buf), 
}
