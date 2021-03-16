CPU=cpu0el # or cpu0 for big endian

CC_DIR=/path/to/cpu0/llvm/bin/clang
CC2_DIR=/path/to/cpu0/bin/llc
LD_DIR=/path/to/cpu0/bin/ld.lld

CC=$(CC_DIR) -c -emit-llvm -fno-stack-protector
CC2=$(CC2_DIR) -march=$(CPU) -filetype=obj
LD=$(LD_DIR) -image-base=0x0 -entry __init

md5.o: md5.c
	$(CC) md5.c -o md5.bc
	$(CC2) md5.bc -o md5.o

clean:
	rm *.bc
	rm *.o
