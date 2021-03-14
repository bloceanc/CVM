CFLAGS=-c -O0 -ggdb3 -DDEBUG

sim: sim.o api.o brk.o file.o printf.o vm.o elf_load.o cpu.o std.o
api.o: api/api.c
	gcc ${CFLAGS} api/api.c -o api.o
brk.o: api/brk.c
	gcc ${CFLAGS} api/brk.c -o brk.o
file.o: api/file.c
	gcc ${CFLAGS} api/file.c -o file.o
printf.o: api/printf.c
	gcc ${CFLAGS} api/printf.c -o printf.o
std.o: api/std.c
	gcc ${CFLAGS} api/std.c -o std.o

sim.o: sim.c
vm.o: vm.c
elf_load.o: elf_load.c
cpu.o: cpu.c
clean:
	rm *.o
	rm sim
