FILES = \
  ./build/kernel.asm.o \
  ./build/kernel.o \
  ./build/terminal/terminal.o \
  ./build/memory/memory.o \
  ./build/idt/idt.asm.o \
  ./build/idt/idt.o

INCLUDES = -I./src
FLAGS = \
  -g \
  -ffreestanding \
  -falign-jumps \
  -falign-functions \
  -falign-labels \
  -falign-loops \
  -fstrength-reduce \
  -fomit-frame-pointer \
  -finline-functions \
  -fno-builtin \
  -nostdlib \
  -nostartfiles \
  -nodefaultlibs \
  -Wall \
  -O0 \
  -Iinc

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
	nasm \
	  -f bin \
	  -o $@ \
	  $<

# Link the kernel

./bin/kernel.bin: ./src/linker.ld ./build/kernelfull.o | bindir
	i686-elf-gcc \
	  $(FLAGS) \
	  -T ./src/linker.ld \
	  -o $@ \
	  ./build/kernelfull.o

./build/kernelfull.o: $(FILES) | builddir
	i686-elf-ld \
	  -g \
	  -relocatable \
	  -o $@ \
	  $^

# NASM builds

./build/kernel.asm.o: ./src/kernel.asm | builddir
	nasm \
	  -f elf \
	  -g \
	  -o $@ \
	  $<

./build/idt/idt.asm.o: ./src/idt/idt.asm | builddir
	mkdir -p build/idt
	nasm \
	  -f elf \
	  -g \
	  -o $@ \
	  $<

# C builds

./build/kernel.o: ./src/kernel.c | builddir
	i686-elf-gcc \
	    $(INCLUDES) \
	    $(FLAGS) \
	    -std=gnu99 \
	    -c $< \
	    -o $@

./build/terminal/terminal.o: ./src/terminal/terminal.c | builddir
	mkdir -p build/terminal
	i686-elf-gcc \
	    $(INCLUDES) \
	    $(FLAGS) \
	    -std=gnu99 \
	    -c $< \
	    -o $@

./build/memory/memory.o: ./src/memory/memory.c | builddir
	mkdir -p build/memory
	i686-elf-gcc \
	    $(INCLUDES) \
	    $(FLAGS) \
	    -std=gnu99 \
	    -c $< \
	    -o $@

./build/idt/idt.o: ./src/idt/idt.c | builddir
	mkdir -p build/idt
	i686-elf-gcc \
	    $(INCLUDES) \
	    $(FLAGS) \
	    -std=gnu99 \
	    -c $< \
	    -o $@


bindir:
	mkdir -p bin

builddir:
	mkdir -p build
