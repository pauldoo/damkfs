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

`qemu-system-x86_64 -drive format=raw,file=./boot.bin`

In `gdb`:

`(gdb) target remote | qemu-system-x86_64 -drive format=raw,file=./boot.bin -S -gdb stdio`

`layout asm`

`info registers`

# Links

http://www.ctyme.com/rbrown.htm

http://www.gabrielececchetti.it/Teaching/CalcolatoriElettronici/Docs/i8086_instruction_set.pdf
