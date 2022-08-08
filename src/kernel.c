
#include "kernel.h"

#include "terminal/terminal.h"
#include "idt/idt.h"
#include "memory/memory.h"
#include "io/io.h"

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

void kernel_main() {
    idt_init();

    terminal_clear(Dark_Gray); 
    terminal_print("Hello world!\n", Light_Green, Dark_Gray);

    initialize_heaps();
    terminal_print("Heaps initialized.\n", Light_Green, Dark_Gray);

    excercise_heap();

    terminal_print("Finished.\n", Light_Green, Dark_Gray);
    halt();
}
