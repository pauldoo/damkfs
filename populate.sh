#!/usr/bin/env fish

set fish_trace on

# Populates the OS filesystem with stuff..
# Expects to run after ./build.sh, but outside the build container.
# Runs outside the container as I haven't figured out loop mounts
# within containers.

mkdir -p ./mnt
sudo mount -t vfat ./bin/os.bin ./mnt
echo "Hello World!" | sudo dd of=./mnt/hello.txt
sudo umount ./mnt
