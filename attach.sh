#!/usr/bin/env sh

gdb \
  -ex 'add-symbol-file build/kernelfull.o 0x00100000' \
  -ex 'break _start' \
  -ex 'target remote :1234'