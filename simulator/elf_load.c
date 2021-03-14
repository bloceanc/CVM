#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory.h>
#include <elf.h>

#include "base.h"
#include "cpu.h"
#include "vm.h"


u32_t load_elf(s32_t fd, vm_t *vm)
{
    Elf32_Ehdr  ehdr;
    Elf32_Phdr *phdr = NULL;
    Elf32_Shdr *shdr = NULL;
    Elf32_Sym  *syms = NULL;
    s32_t i, n, symn = 0;
    u32_t addr, link;
    s8_t *strings = NULL;
    s8_t *mem = vm->mem.start;
    s8_t *end = vm->mem.end;

    lseek(fd, 0, SEEK_SET);
    n = read(fd, &ehdr, sizeof(Elf32_Ehdr));
    if (n != sizeof(Elf32_Ehdr)) {
        printf("load elf header failed\n");
        goto failed;
    }

    phdr = (Elf32_Phdr *)malloc(sizeof(*phdr) * ehdr.e_phnum);
    if (!phdr) {
        printf("alloc memory failed.\n");
        goto failed;
    }
    lseek(fd, ehdr.e_phoff, SEEK_SET);
    n = read(fd, phdr, sizeof(*phdr) * ehdr.e_phnum);
    if (n != (s32_t)(sizeof(*phdr) * ehdr.e_phnum)) {
        printf("read ph failed\n");
        goto failed;
    }

    for (i = 0; i < ehdr.e_phnum; i++) {
        if (phdr[i].p_type != PT_LOAD) {
            continue;
        }

        if ((mem + phdr[i].p_vaddr + phdr[i].p_memsz) >= end) {
            printf("no memory load program\n");
            goto failed;
        }

        lseek(fd, phdr[i].p_offset, SEEK_SET);
        n = read(fd, mem + phdr[i].p_vaddr, phdr[i].p_filesz);
        if (n != (s32_t)(phdr[i].p_filesz)) {
            printf("read segment failed\n");
            goto failed;
        }
        addr = (phdr[i].p_vaddr + phdr[i].p_memsz + phdr[i].p_align - 1) & ~(phdr[i].p_align - 1);
    }

    vm->mem.brk = (void *)(unsigned long)addr;

    /* find symbol & string table */
    lseek(fd, ehdr.e_shoff, SEEK_SET);
    shdr = (Elf32_Shdr *)malloc(sizeof(*shdr) * ehdr.e_shnum);
    if (!shdr) {
        printf("no memory \n");
        goto failed;
    }

    n = read(fd, shdr, sizeof(*shdr) * ehdr.e_shnum);
    if (n != (s32_t)(sizeof(*shdr) * ehdr.e_shnum)) {
        printf("read data failed\n");
        goto failed;
    }

    for (i = 0; i < ehdr.e_shnum; i++) {
        if (shdr[i].sh_type == SHT_SYMTAB) {
            link = shdr[i].sh_link;
            syms = (void *)(unsigned long)shdr[i].sh_addr;
            symn = shdr[i].sh_size / shdr[i].sh_entsize;

            break;
        }
    }


    if (!symn) {
        goto failed;
    }

    strings = (char *)malloc(shdr[link].sh_size);
    if (0 > lseek(fd, shdr[link].sh_offset, SEEK_SET)) {
        goto failed;
    }
    if (shdr[link].sh_size != read(fd, strings, shdr[link].sh_size)) {
        goto failed;
    }

    syms = (Elf32_Sym *)malloc(shdr[i].sh_size);
    if (0 > lseek(fd, shdr[i].sh_offset, SEEK_SET)) {
        goto failed;
    }
    if (shdr[i].sh_size != read(fd, syms, shdr[i].sh_size)) {
        goto failed;
    }

    vm->elf.sym_addr = (void *)syms;
    vm->elf.str_addr = strings;
    vm->elf.sym_count = symn;

    free(phdr);
    free(shdr);
    return ehdr.e_entry;

failed:
    if (phdr) {
        free(phdr);
    }

    if (shdr) {
        free(shdr);
    }

    return (u32_t)-1;
}

u32_t elf_resolve(vm_t *vm, s8_t *symbol)
{
    u32_t i;
    Elf32_Sym  *sym = (Elf32_Sym *)vm->elf.sym_addr;

    for (i = 0; i < vm->elf.sym_count; i++) {
        if(0 == strcmp(&vm->elf.str_addr[sym[i].st_name], symbol)) {
            return sym[i].st_value;
        }
    }

    return (u32_t)-1;
}

