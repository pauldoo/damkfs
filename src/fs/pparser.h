#pragma once

typedef struct fs_path_t {
    const char* part;
    struct fs_path_t* next;
} fs_path;

fs_path* path_parse(const char* path);

void path_free(fs_path* path);
