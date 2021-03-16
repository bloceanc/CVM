#include "api.h"
#include "base.h"


int open(const char *pathname, int flags)
{
    int ret;

    RT_API_WITH_RET(API_OPEN, ret);
    return ret;
}

int read(int fd, char *buf, int count)
{
    int ret;

    RT_API_WITH_RET(API_READ, ret);
    return ret;
}

int write(int fd, char *buf, int count)
{
    int ret;
    
    RT_API_WITH_RET(API_WRITE, ret);
    return ret;
}

int close(int fd)
{
    int ret;

    RT_API_WITH_RET(API_CLOSE, ret);
    return ret;
}

int lseek(int fd, int offset, int whence)
{
    int ret;

    RT_API_WITH_RET(API_WRITE, ret);
    return ret;
}

