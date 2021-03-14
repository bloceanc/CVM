#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>

#include "base.h"
#include "cpu.h"
#include "vm.h"

#define STACK_SAFE_HOLE	0x100

extern u32_t load_elf(s32_t fd, vm_t *vm);
extern s32_t run_loop(vm_t *vm, u32_t tid);

static s32_t load_program(s32_t fd, vm_t *vm)
{
    s32_t r;

    r = load_elf(fd, vm);
    if (r == -1 ) {
        printf("load elf failed\n");
        return -1;
    }

    vm->entry = r;
    return 0;
}

static s32_t vm_init(vm_t *vm, u32_t memsize)
{
    s8_t *t = (s8_t *)malloc(memsize);
    if (!t) {
        return -1;
    }
    cpu_init(&vm->cpu);
    vm->mem.start = t;
    vm->mem.end = t + memsize;

    return 0;
}


static s32_t vm_load(vm_t *vm, s32_t fd)
{
    s32_t r;
    r = load_program(fd, vm);
    if (r < 0) {
        return -1;
    }
    return 0;
}

u32_t abi_func_param_n(vm_t *vm, u32_t tid, u32_t idx)
{
    if (idx == 1) {
        return vm->cpu.core.regs[4];
    }

    if (idx == 2) {
        return vm->cpu.core.regs[5];
    }

    if (idx > 2) {
        return *(u32_t *)(vm->mem.start + vm->threads[tid].prev_sp + 4 + (idx - 2) * 4);
    }

    return (u32_t)-1;
}

void abi_func_set_return(vm_t *vm, u32_t ret)
{
    /* !!!!! we use r3, becuase r2 used to save return value, and can not change in API call, the asm code, I could not change it .... !!!!*/
    vm->cpu.core.regs[3] = ret;
}

u32_t abi_func_get_return_int(vm_t *vm, u32_t tid)
{
    return vm->cpu.core.regs[2];
}

void *abi_func_get_return_addr(vm_t *vm, u32_t tid)
{
    return vm->mem.start + vm->cpu.core.regs[2];
}

s32_t vm_thread_init(vm_t *vm, u32_t tid, u32_t func_addr)
{
    /**
     * low -------------------  start -
     *            .text
     *     --------------------
     *            .data
     *     --------------------
     *            .bss
     *     --------------------
     *            .brk
     *     --------------------
     *           for heap
     *     --------------------
     *        thread n stack
     *     --------------------
     *       thread ... stack
     *     --------------------
     *       thread  2  stack
     *     --------------------
     *       thread  1  stack
     *     --------------------
     *       thread  0  stack
     *
     * high-------------------- end -
     *
     *  thread stack size are fixed and preserved
     */
    thread_ctx_t *thread = &vm->threads[tid];

    thread->entry = (void *)(unsigned long)func_addr;  /* relative address from mem.start */
    thread->sp = (s8_t *)(vm->mem.end - vm->mem.start) - tid * vm->stack_size - STACK_SAFE_HOLE; /* relative address from mem.start */
    thread->sp_end = (s8_t *)(vm->mem.end - vm->mem.start) - (tid + 1) * vm->stack_size; /* relative address from mem.start, must be precise, used to auto-end loop! */

    return 0;
}

vm_t *vm_create(s8_t *bin, u32_t memsize, u32_t stacksize)
{
    s32_t fd = open(bin, O_RDWR);
    vm_t *vm = NULL;

    if (fd < 0) {
        printf("open file failed\n");
        goto failed;
    }

    if (stacksize * 2 < STACK_SAFE_HOLE) {
        printf("stack size too small, at least 1024\n");
	goto failed;
    }

    vm = (vm_t *)malloc(sizeof(vm_t));
    if (!vm) {
        printf("alloc memory failed\n");
        goto failed;
    }

    memset(vm, 0, sizeof(*vm));

    if (0 > vm_init(vm, memsize)) {
        goto failed;
    }

    if (0 > vm_load(vm, fd)) {
        goto failed;
    }

    vm->stack_size = stacksize;
    return vm;

failed:
    if (fd > 0) {
        close(fd);
    }
    if (vm) {
        free(vm);
    }

    return NULL;
}

s32_t vm_create_main_thread(vm_t *vm, u32_t func)
{
    if (!func) {
        func = vm->entry;
    }

    if (0 < vm_thread_init(vm, 0, func)) {
        printf("create main thread failed\n");
        return -2;
    }

    return 0;
}

/* do not support any pointer parameters */
void vm_push_main_thread_param(vm_t *vm, s32_t param)
{
    if (vm->param_count == 0) {
        vm->cpu.core.regs[4] = param;
    }

    if (vm->param_count == 1) {
        vm->cpu.core.regs[5] = param;
    }

    *(((s32_t *)((u32_t)(unsigned long)vm->threads[0].sp + vm->mem.start)) + vm->param_count) = param;

    vm->param_count++;
    return;
}

s32_t vm_start(vm_t *vm, u32_t tid)
{
    s32_t r;
/*
    if (0 < vm_thread_init(vm, 0, vm->entry)) {
        printf("create main thread failed\n");
        return -2;
    }
*/
    /* run main thread */
    r = run_loop(vm, tid);
    vm->param_count = 0;
    return r;
}

