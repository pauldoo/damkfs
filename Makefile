all: boot.bin

clean:
	rm -f ./boot.bin

boot.bin: boot.asm message.txt
	nasm -f bin -o ./boot.bin ./boot.asm
	dd if=./message.txt >> ./boot.bin
	dd if=/dev/zero bs=512 count=1 >> ./boot.bin
