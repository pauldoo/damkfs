all: ./bin/boot.bin

clean:
	rm -rf ./bin ./build

./bin/boot.bin: ./src/boot/boot.asm
	mkdir -p bin
	nasm -f bin -o ./bin/boot.bin ./src/boot/boot.asm
