#include "memory.h"

void memzero(void* p, size_t len) {
    for (size_t i = 0; i < len; i++) {
        ((char*)p)[i] = 0;
    }
}

int strlen(const char* str) {
    int len = 0;
    while (str[len] != 0) {
        len++;
    }
    return len;
}
