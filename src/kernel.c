
#include "kernel.h"

#include "vga_terminal.h"


void kernel_main() {
    terminal_clear();

    terminal_put_string(terminal_width / 2, terminal_height / 2, "Hello World!!!", Yellow, Blue);
    terminal_print("Hello\nWorld!\n", Light_Green, Blue);
}
