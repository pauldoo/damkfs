
#include "kernel.h"

#include "terminal/terminal.h"
#include "idt/idt.h"
#include "memory/memory.h"
#include "io/io.h"
#include "memory/paging.h"

static void excercise_heap() {
    int* a = kmalloc(4);
    int* b = kmalloc(4);
    *a = 'A';
    *b = 'B';

    if (*a == 'A' && *b == 'B') {
        dprint_str("success?!\n");
    } else {
        dprint_str("no success?!\n");
    }

    kfree(a);
    a = NULL;

    a = kmalloc(100);
    *a = 'C';
    if (*a == 'C' && *b == 'B') {
        dprint_str("success2?!\n");
    } else {
        dprint_str("no success?!\n");
    }
    kfree(b);
    b = NULL;

    int *c = kmalloc(4);
    *c = 'K';
    if (*a == 'C' && *c == 'K') {
        dprint_str("success3?!\n");
    } else {
        dprint_str("no success?!\n");
    }


    dprint_str("End.\n");    
}

static page_directory* kernel_pd = 0;

void kernel_main() {
    // Hello
    dprint_clear(); 
    dprint_str("Hello world!\n");

    // Heaps
    initialize_heaps();
    dprint_str("Heaps initialized.\n");

    // Paging
    kernel_pd = page_directory_create();
    dprint_str("Kernel page directory allocated.\n");
    page_directory_populate_linear(kernel_pd, paging_is_present | paging_is_writeable | paging_access_from_all);
    dprint_str("Kernel page directory populated.\n");
    page_directory_switch(kernel_pd);
    dprint_str("Kernel page directory selected.\n");

    // Temporary: use page tables to demonstrate aliasing

    uint32_t* numberA = page_alloc(1);
    uint32_t* numberB = page_alloc(1);
    dprint_hex((uint32_t)numberA);
    dprint_str("\n");
    dprint_hex((uint32_t)numberB);
    dprint_str("\n");

    ASSERT( numberA != numberB );
    page_set(kernel_pd, numberB, ((uint32_t)numberA) | paging_is_present | paging_is_writeable | paging_access_from_all);

    *numberA = 10;
    *numberB = 20;

    dprint_dec(*numberA);
    dprint_str("\n");
    dprint_dec(*numberB);
    dprint_str("\n");

    enable_paging();
    dprint_str("Paging enabled.\n");

    dprint_dec(*numberA);
    dprint_str("\n");
    dprint_dec(*numberB);
    dprint_str("\n");

    *numberA = 30;

    dprint_dec(*numberA);
    dprint_str("\n");
    dprint_dec(*numberB);
    dprint_str("\n");

    // Enable interrupts
    idt_init();



    // Test
    excercise_heap();

    // Fin.
    dprint_str("Finished.\n");
    halt();
}
