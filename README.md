# Intro

My code and notes from following the Udemy course [developing a multithreaded kernel from scratch](https://www.udemy.com/course/developing-a-multithreaded-kernel-from-scratch/).



# Development

## Building

`./build.sh ;and ./populate.sh`

## Running

`./run.sh`


Or to attach a debugger:
1. `./run.sh` (will pause on start)
2. From a different shell: `./attach.sh`

## Disassemble

`ndisasm ./boot.bin`

## GDB cheatsheet

```
layout asm

info registers
```

# Resources

## GCC cross compiler

https://wiki.osdev.org/GCC_Cross-Compiler

Docker container seems easiest:

https://hub.docker.com/r/joshwyant/gcc-cross



## Links

https://github.com/nibblebits/PeachOS

http://www.ctyme.com/rbrown.htm

http://www.gabrielececchetti.it/Teaching/CalcolatoriElettronici/Docs/i8086_instruction_set.pdf

https://wiki.osdev.org/

https://wiki.osdev.org/CPU_Registers_x86

https://wiki.osdev.org/ATA_read/write_sectors