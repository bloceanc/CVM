#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../base.h"
#include "../cpu.h"
#include "../vm.h"
#include "api.h"


#define F_O_RDONLY  1
#define F_O_WRONLY  2
#define F_O_RDWR    3
#define F_O_CREAT   4

#define F_SEEK_SET  0
#define F_SEEK_END  1
#define F_SEEK_CUR  2

/**
 * open(const char *filename. int flag)
 */
s32_t cpu0_open(vm_t *vm, u32_t tid, ...)
{
    u32_t filename_n = abi_func_param_n(vm, tid, 1);
    s32_t flag = abi_func_param_n(vm, tid, 2);
    s8_t  *filename = (s8_t *)ABI_PARAM_TO_ADDR(vm, filename_n);
    s32_t plt_flag, ret;

    switch (flag & 3) {
        case F_O_RDONLY:
            plt_flag = O_RDONLY;
            break;
        case F_O_WRONLY:
            plt_flag = O_WRONLY;
            break;
        case F_O_RDWR:
            plt_flag = O_RDWR;
            break;
        default:
            ret = -1;
            break;
    }

    if (ret < 0) {
        goto done;
    }

    if (flag & 4) {
        plt_flag |= O_CREAT;
    }

    ret = open(filename, plt_flag);

done:
    abi_func_set_return(vm, ret);
    return 0;
}


/**
 * ssize_t read(int fd, void *buf, size_t count)
 */
s32_t cpu0_read(vm_t *vm, u32_t tid, ...)
{
    s32_t fd = abi_func_param_n(vm, tid, 1);
    u32_t bufv = abi_func_param_n(vm, tid, 2);
    s32_t size = abi_func_param_n(vm, tid, 3);
    s8_t *buf_addr = ABI_PARAM_TO_ADDR(vm, bufv);
    s32_t ret;

    ret = read(fd, buf_addr, size);

    abi_func_set_return(vm, ret);
    return 0;
}

/**
 * ssize_t write(int fd, const void *buf, size_t count)
 */
s32_t cpu0_write(vm_t *vm, u32_t tid, ...)
{
    s32_t fd = abi_func_param_n(vm, tid, 1);
    u32_t bufv = abi_func_param_n(vm, tid, 2);
    s32_t size = abi_func_param_n(vm, tid, 3);
    s8_t *buf_addr = ABI_PARAM_TO_ADDR(vm, bufv);
    s32_t ret;

    ret = write(fd, buf_addr, size);

    abi_func_set_return(vm, ret);
    return 0;
}

/**
 * int close(int fd);
 */
s32_t cpu0_close(vm_t *vm, u32_t tid, ...)
{
    s32_t fd = abi_func_param_n(vm, tid, 1);
    s32_t ret;

    ret = close(fd);

    abi_func_set_return(vm, ret);
    return 0;
}

/**
 * off_t lseek(int fd, off_t offset, int whence);
 */
s32_t cpu0_lseek(vm_t *vm, u32_t tid, ...)
{
    s32_t fd = abi_func_param_n(vm, tid, 1);
    s32_t offset = abi_func_param_n(vm, tid, 2);
    s32_t whence = abi_func_param_n(vm, tid, 3);
    s32_t ret, plt_whence;

    switch (whence) {
        case F_SEEK_SET:
            plt_whence = SEEK_SET;
            break;
        case F_SEEK_END:
            plt_whence = SEEK_END;
            break;
        case F_SEEK_CUR:
            plt_whence = SEEK_CUR;
            break;
        default:
            plt_whence = whence;
            break;
    }

    ret = lseek(fd, offset, plt_whence);
    abi_func_set_return(vm, ret);
    return 0;
}


