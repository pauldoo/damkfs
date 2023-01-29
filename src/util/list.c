#include "list.h"

#include "memory/memory.h"
#include "terminal/terminal.h"

list* list_create() {
    list* result = kmalloc(sizeof(list));
    result->_data = 0;
    result->_allocated = 0;
    result->_used = 0;

    return result;
}

void list_free(list* list) {
    if (list->_data != 0) {
        kfree(list->_data);
    }
    kfree(list);
}

uint32_t list_size(list* list) {
    return list->_used;
}

static void increase_capacity(list* list) {
    uint32_t new_allocation = list->_allocated * 2;
    if (new_allocation < 8) {
        new_allocation = 8;
    }

    uint32_t* new_data = kmalloc(new_allocation * sizeof(uint32_t));
    for (uint32_t i = 0; i < list->_used; i++) {
        new_data[i] = list->_data[i];
    }
    if (list->_data != 0) {
        kfree(list->_data);
    }
    list->_data = new_data;
    list->_allocated = new_allocation;
}

void list_append(list* list, uint32_t value) {
    if (list->_used >= list->_allocated) {
        increase_capacity(list);
    }
    ASSERT(list->_used < list->_allocated);
    list->_data[list->_used] = value;
    list->_used += 1;
}

uint32_t list_at(list* list, uint32_t index) {
    ASSERT(list != NULL);
    ASSERT(index < list->_used);

    return list->_data[index];
}