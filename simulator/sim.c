#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "cpu.h"
#include "vm.h"

extern u32_t elf_resolve(vm_t *vm, s8_t *symbol);

int main(void)
{
    vm_t *pvm = NULL;
    char *challenge = NULL;
    unsigned int symbol;

    pvm = vm_create("./lic", 4 * 1024 * 1024, 1024);
    if (!pvm) {
        printf("create vm failed\n");
        return -1;
    }

    /* call __init */
    symbol = elf_resolve(pvm, "__init");
    if (0 != vm_create_main_thread(pvm, symbol)) {
        printf("create main thread failed\n");
        return -1;
    }
    vm_start(pvm, 0);

    /* call load_file */
    symbol = elf_resolve(pvm, "load_file");
    if (0 != vm_create_main_thread(pvm, symbol)) {
        printf("create main thread failed\n");
        return -1;
    }
    vm_push_main_thread_param(pvm, 0);
    vm_start(pvm, 0);

    /* get challenge */
    symbol = elf_resolve(pvm, "get_challenge_pwd");
    if (0 != vm_create_main_thread(pvm, symbol)) {
        printf("create main thread failed\n");
        return -1;
    }
    vm_start(pvm, 0);
    challenge = abi_func_get_return_addr(pvm, 0);
    printf("challenge is %s\n", challenge);

    return 0;
}
