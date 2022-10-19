
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
        terminal_print("success?!\n", White, Black);
    } else {
        terminal_print("no success?!\n", White, Black);
    }

    kfree(a);
    a = NULL;

    a = kmalloc(100);
    *a = 'C';
    if (*a == 'C' && *b == 'B') {
        terminal_print("success2?!\n", White, Black);
    } else {
        terminal_print("no success?!\n", White, Black);
    }
    kfree(b);
    b = NULL;

    int *c = kmalloc(4);
    *c = 'K';
    if (*a == 'C' && *c == 'K') {
        terminal_print("success3?!\n", White, Black);
    } else {
        terminal_print("no success?!\n", White, Black);
    }


    terminal_print("End.\n", White, Black);    
}

static page_directory* kernel_pd = 0;

void kernel_main() {
    // Hello
    terminal_clear(Dark_Gray); 
    terminal_print("Hello world!\n", Light_Green, Dark_Gray);

    terminal_print_dec(12340, Light_Green, Dark_Gray);
    terminal_print("\n", Light_Green, Dark_Gray);
    terminal_print_dec(0, Light_Green, Dark_Gray);
    terminal_print("\n", Light_Green, Dark_Gray);
    terminal_print_hex(0x12abc, Light_Green, Dark_Gray);
    terminal_print("\n", Light_Green, Dark_Gray);
    terminal_print_hex(0x0, Light_Green, Dark_Gray);
    terminal_print("\n", Light_Green, Dark_Gray);

    // Heaps
    initialize_heaps();
    terminal_print("Heaps initialized.\n", Light_Green, Dark_Gray);

    // Paging
    kernel_pd = page_directory_create();
    terminal_print("Kernel page directory allocated.\n", Light_Green, Dark_Gray);
    page_directory_populate_linear(kernel_pd, paging_is_present | paging_is_writeable | paging_access_from_all);
    terminal_print("Kernel page directory populated.\n", Light_Green, Dark_Gray);
    page_directory_switch(kernel_pd);
    terminal_print("Kernel page directory selected.\n", Light_Green, Dark_Gray);
    enable_paging();
    terminal_print("Paging enabled.\n", Light_Green, Dark_Gray);

    // Enable interrupts
    idt_init();



    // Test
    excercise_heap();

    // Fin.
    terminal_print("Finished.\n", Light_Green, Dark_Gray);
    halt();
}
