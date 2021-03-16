#include "api.h"
#include "base.h"

void *sbrk(long increment)
{
    void *ret;
    
    RT_API_WITH_RET(API_SBRK, ret);
    return ret;
}

int brk(void *addr)
{
    int ret;

    RT_API_WITH_RET(API_BRK, ret);
    return ret;
}
