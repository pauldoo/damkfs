#!/usr/bin/env bash

# Expects to run inside the gcc-cross container.

set -e
set -x

export PREFIX="/usr/local/cross"
export TARGET="i686-elf"

make clean

make
