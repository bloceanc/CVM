#include "api.h"
#include "base.h"

int printf(char *fmt, ...)
{
    int ret;

    RT_API_WITH_RET(API_PRINTF, ret);
    return ret;
}

/*
int main(void)
{
    return printf("%d\n", 0x1234);
}
*/
