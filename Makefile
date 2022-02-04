FILES = ./build/kernel.asm.o

all: ./bin/os.bin

./bin/os.bin: ./bin/boot.bin ./bin/kernel.bin
	rm -f ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=512 count=100 >> ./bin/os.bin

clean:
	rm -rf ./bin ./build


./bin/boot.bin: ./src/boot/boot.asm
	mkdir -p bin
	nasm \
	  -f bin \
	  -o ./bin/boot.bin \
	  ./src/boot/boot.asm

#	echo -n -e 'Hello from 2nd sector!\0' >> ./bin/boot.bin
#	dd if=/dev/zero bs=512 count=1 >> ./bin/boot.bin

./bin/kernel.bin: ./src/linker.ld ./build/kernelfull.o
	mkdir -p bin
	i686-elf-gcc \
	  -T ./src/linker.ld \
	  -o ./bin/kernel.bin \
	  -ffreestanding \
	  -O0 \
	  -nostdlib \
	  ./build/kernelfull.o

./build/kernelfull.o: $(FILES)
	mkdir -p build
	i686-elf-ld \
	  -g \
	  -relocatable \
	  -o ./build/kernelfull.o \
	  $(FILES)

./build/kernel.asm.o: ./src/kernel.asm
	mkdir -p build
	nasm \
	  -f elf \
	  -g \
	  -o ./build/kernel.asm.o \
	  ./src/kernel.asm 

