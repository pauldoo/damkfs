#!/usr/bin/env sh

qemu-system-x86_64 -m 1G -drive format=raw,file=./bin/os.bin -S -s
