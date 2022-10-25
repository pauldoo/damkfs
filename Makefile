FILES = \
  ./build/kernel.asm.o \
  ./build/kernel.o \
  ./build/terminal/terminal.o \
  ./build/memory/memory.o \
  ./build/memory/heap.o \
  ./build/memory/paging.asm.o \
  ./build/memory/paging.o \
  ./build/idt/idt.asm.o \
  ./build/idt/idt.o \
  ./build/io/io.asm.o

INCLUDES = -I./src
FLAGS = \
  -g \
  -Og \
  -ffreestanding \
  -fno-builtin \
  -nostdlib \
  -nostartfiles \
  -nodefaultlibs \
  -Wall \
  -Werror \
  -Iinc

.PHONY: all clean
all: ./bin/os.bin

./bin/os.bin: ./bin/boot.bin ./bin/kernel.bin | bindir
	rm -f $@
	dd if=./bin/boot.bin >> $@
	dd if=./bin/kernel.bin >> $@
	dd if=/dev/zero bs=512 count=100 >> $@

clean:
	rm -rf ./bin ./build

# Bootloader

./bin/boot.bin: ./src/boot/boot.asm | bindir
	nasm -f bin -o $@ $<

# Link the kernel

./bin/kernel.bin: ./src/linker.ld $(FILES) | bindir
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o $@ \
	  $(FILES)

# NASM builds

./build/%.asm.o: ./src/%.asm | builddir
	nasm -f elf -g -o $@ $<

# C builds

./build/%.o: ./src/%.c | builddir
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -o $@ -c $<

# Create output directories

bindir:
	mkdir -p bin

builddir:
	mkdir -p build build/terminal build/memory build/idt build/io
