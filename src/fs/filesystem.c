#include "filesystem.h"

#include "terminal/terminal.h"

bdev* filesystem_open_file(filesystem* fs, fs_path* path) {
    ASSERT(fs != 0);
    ASSERT(path != 0);
    return fs->vtable->open_file(fs, path);
}

void filesystem_close(filesystem* fs) {
    ASSERT(fs != 0);
    fs->vtable->close(fs);
}
