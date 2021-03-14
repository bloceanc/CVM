#ifndef _SIMULATOR_VM_H_
#define _SIMULATOR_VM_H_

#include "base.h"
#include "cpu.h"

#define MAX_THREAD  32

#define THREAD_FLAG_NULL    0x0
#define THREAD_FLAG_RUNNING 0x1
#define THREAD_FLAG_SCHDOUT 0x2
#define THREAD_FLAG_WAITING 0x3

typedef struct elf_info_s {
    s8_t *sym_addr;
    u32_t sym_count;
    s8_t *str_addr;
} elf_info_t;

typedef struct memory_s {
    s8_t *start;
    s8_t *end;
    s8_t *brk;
} memory_t;

typedef struct thread_ctx_s {
    cpu_t   cpu_ctx;    /* only used for save CPU context when task scheduled out/in */
    u32_t   flag;
    reg_t   prev_sp; /* used to get parameters in stack */
    s8_t    *entry;
    s8_t    *sp;
    s8_t    *sp_end;
} thread_ctx_t;

typedef struct vm_s {
    elf_info_t      elf;
    cpu_t           cpu;
    memory_t        mem;
    u32_t           entry;
    u32_t           stack_size;
    u32_t           param_count;
    thread_ctx_t    threads[MAX_THREAD];
} vm_t;

extern u32_t abi_func_param_n(vm_t *vm, u32_t tid, u32_t n/*start from 1*/);
extern void abi_func_set_return(vm_t *vm, u32_t ret);
extern u32_t abi_func_get_return_int(vm_t *vm, u32_t tid);
extern void* abi_func_get_return_addr(vm_t *vm, u32_t tid);

#define ABI_PARAM_TO_ADDR(vm, p) ((vm)->mem.start + (u32_t)(p))
#define ABI_ADDR_TO_PARAM(vm, addr) (u32_t)((unsigned long)addr - (unsigned long)(vm)->mem.start) 

extern vm_t *vm_create(s8_t *bin, u32_t memsize, u32_t stack_size);
extern u32_t vm_load_symbol(vm_t *vm, s8_t *symbol);
extern s32_t vm_create_main_thread(vm_t *vm, u32_t func);
extern void vm_push_main_thread_param(vm_t *vm, s32_t param);
extern s32_t vm_start(vm_t *vm, u32_t tid);

#endif /* _SIMULATOR_VM_H_ */

