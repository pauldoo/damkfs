#!/usr/bin/env fish

set fish_trace on

# Populates the OS filesystem with stuff..
# Expects to run after ./build.sh, but outside the build container.
# Runs outside the container as I haven't figured out loop mounts
# within containers.

mkdir -p ./mnt
sudo mount -t vfat -o rw,uid=(id -u),gid=(id -g) ./bin/os.bin ./mnt

echo "Hello World!" | dd of=./mnt/hello.txt

mkdir -p ./mnt/sub/dir
find ./src/ -type f | xargs -- cat | dd of=./mnt/sub/dir/source.txt

sudo umount ./mnt
