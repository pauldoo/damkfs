all: ./bin/boot.bin

clean:
	rm -rf ./bin ./build

./bin/boot.bin: ./src/boot/boot.asm
	mkdir -p bin
	nasm -f bin -o ./bin/boot.bin ./src/boot/boot.asm
	echo -n -e 'Hello from 2nd sector!\0' >> ./bin/boot.bin
	dd if=/dev/zero bs=512 count=1 >> ./bin/boot.bin
