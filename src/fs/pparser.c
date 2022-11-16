#include "pparser.h"

#include "terminal/terminal.h"
#include "memory/memory.h"

fs_path* path_parse(const char* path) {
    ASSERT(path[0] == '/');

    path += 1;

    const char* path_end = path;
    while (*path_end != 0 && *path_end != '/') {
        path_end += 1;
    }

    const int fragment_length = path_end - path;
    ASSERT(fragment_length > 0);

    char* const fragment_copy = kmalloc(fragment_length + 1);
    for (int i = 0; i < fragment_length; i++) {
        fragment_copy[i] = path[i];
    }
    fragment_copy[fragment_length] = 0;

    fs_path* const result = kmalloc(sizeof(fs_path));
    result->part = fragment_copy;
    result->next = 0;

    if (*path_end == '/') {
        result->next = path_parse(path_end);
    }

    return result;
}

void path_free(fs_path* path) {
    ASSERT(path != 0);

    ASSERT(path->part != 0);
    kfree(path->part);

    if (path->next != 0) {
        path_free(path->next);
    }
}
