FILES = ./build/kernel.asm.o ./build/kernel.o ./build/vga_terminal.o
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
  -Wno-unused-function \
  -fno-builtin \
  -Werror \
  -Wno-unused-label \
  -Wno-cpp \
  -Wno-unused-parameter \
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


./bin/boot.bin: ./src/boot/boot.asm | bindir
	nasm \
	  -f bin \
	  -o $@ \
	  $<

#	echo -n -e 'Hello from 2nd sector!\0' >> ./bin/boot.bin
#	dd if=/dev/zero bs=512 count=1 >> ./bin/boot.bin

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

./build/kernel.asm.o: ./src/kernel.asm | builddir
	nasm \
	  -f elf \
	  -g \
	  -o $@ \
	  $<

./build/kernel.o: ./src/kernel.c | builddir
	i686-elf-gcc \
	    $(INCLUDES) \
	    $(FLAGS) \
	    -std=gnu99 \
	    -c $< \
	    -o $@

./build/vga_terminal.o: ./src/vga_terminal.c | builddir
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
