CPU=cpu0el # or cpu0 for big endian

CC_DIR=/path/to/cpu0/bin/clang
CC2_DIR=/path/to/cpu0/bin/llc
AR_DIR=/path/to/cpu0/bin/llvm-ar

CC=$(CC_DIR) -c -emit-llvm -fno-stack-protector
CC2=$(CC2_DIR) -march=$(CPU) -filetype=obj
AR=$(AR_DIR) rv

BSRC=brk.bc file.bc mem.bc printf.bc std.bc
OBJS=brk.o file.o mem.o printf.o std.o

lib.a: $(OBJS)
	$(AR) lib.a $(OBJS)

$(OBJS): $(BSRC)
	$(CC2) brk.bc -o brk.o
	$(CC2) file.bc -o file.o
	$(CC2) mem.bc -o mem.o
	$(CC2) printf.bc -o printf.o
	$(CC2) std.bc -o std.o

brk.bc: brk.c api.h base.h
	$(CC) brk.c -o brk.bc

file.bc: file.c api.h base.h
	$(CC) file.c -o file.bc

mem.bc: mem.c mem.h api.h base.h
	$(CC) mem.c -o mem.bc

printf.bc: printf.c api.h base.h
	$(CC) printf.c -o printf.bc

std.bc: std.c api.h base.h
	$(CC) std.c -o std.bc

clean:
	rm *.bc
	rm *.o
	rm *.a
