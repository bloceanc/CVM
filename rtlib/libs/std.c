#include "api.h"
#include "base.h"

int time(void)
{
    int ret;

    RT_API_WITH_RET(API_TIME, ret);
    return ret;
}
