# Intro

My code and notes from following the Udemy course [developing a multithreaded kernel from scratch](https://www.udemy.com/course/developing-a-multithreaded-kernel-from-scratch/).


# Tools

`dnf install qemu-system-x86 nasm wine`

To install emu8086: `wine setup.exe`

To run emu8086: `wine 'c:/emu8086/emu8086.exe'`

# Building

`nasm -f bin -o ./boot.bin ./boot.asm`

## Disassemble

`ndisasm ./boot.bin`


# Running

## QEMU

`qemu-system-x86_64 -drive format=raw,file=./bin/os.bin`

## GDB

In `gdb`:

```
add-symbol-file build/kernelfull.o 0x00100000

break _start

target remote | qemu-system-x86_64 -drive format=raw,file=./bin/os.bin -S -gdb stdio
```

```
layout asm

info registers
```

## GCC cross compiler

https://wiki.osdev.org/GCC_Cross-Compiler

Docker container seems easiest:

https://hub.docker.com/r/joshwyant/gcc-cross

Enter container: `./enter.sh`

Inside the container: `./build.sh`


# Links

https://github.com/nibblebits/PeachOS

http://www.ctyme.com/rbrown.htm

http://www.gabrielececchetti.it/Teaching/CalcolatoriElettronici/Docs/i8086_instruction_set.pdf

https://wiki.osdev.org/

https://wiki.osdev.org/CPU_Registers_x86

https://wiki.osdev.org/ATA_read/write_sectors