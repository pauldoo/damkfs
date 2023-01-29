#pragma once

#include "disk/bdev.h"
#include "fs/pparser.h"

struct filesystem_vtable_t;
typedef struct filesystem_vtable_t filesystem_vtable;

struct filesystem_t;
typedef struct filesystem_t filesystem;

typedef bdev* (*FilesystemOpenFile)(filesystem*, fs_path*);
typedef void (*FilesystemClose)(filesystem*);

struct filesystem_vtable_t {
    FilesystemOpenFile open_file;
    FilesystemClose close;
};

struct filesystem_t {
    const filesystem_vtable* vtable;
};

bdev* filesystem_open_file(filesystem*, fs_path*);

void filesystem_close(filesystem*);
