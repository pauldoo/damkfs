#include "terminal.h"

const int terminal_width = 80;
const int terminal_height = 25;

typedef struct video_data_t {
    volatile uint16_t data[25][80];
} video_data;

static video_data* const  video_mem = (video_data*)(0xB8000);

printer default_printer = {
    .x = 0,
    .y = 0,
    .fg = Light_Green,
    .bg = Dark_Gray
};

static uint16_t make_char(char c, terminal_color fg, terminal_color bg) {
    return c | (bg << 12) | fg << 8;
}

void terminal_put_char(int x, int y, char c, terminal_color fg, terminal_color bg) {
    if (0 <= x && x < terminal_width &&
        0 <= y && y < terminal_height) {

        video_mem->data[y][x] = make_char(c, fg, bg);
    }
}


void print_clear(printer* p) {
    for (int y = 0; y < terminal_height; y++) {
        for (int x = 0; x < terminal_width; x++) {
            terminal_put_char(x, y, ' ', p->fg, p->bg);
        }
    }
    p->x = 0;
    p->y = 0;
}

void print_char(printer* p, char c) {
    ASSERT(p != 0)
    ASSERT(p->x >= 0 && p->x < terminal_width)
    ASSERT(p->y >= 0 && p->y < terminal_width)

    if (c != '\n') {
        terminal_put_char(p->x, p->y, c, p->fg, p->bg);
        p->x++;
    }

    if (c == '\n' || p->x >= terminal_width) {
        p->x = 0;

        p->y++;
        if (p->y >= terminal_height) {
            p->y = 0;
        }
        for (int x = 0; x <= terminal_width; x++) {
            terminal_put_char(x, p->y, ' ', p->fg, p->bg);
        }
    }
}

void print_str(printer* p, const char* s) {
    while (*s != 0) {
        print_char(p, *s);
        s++;
    }
}

static void print_failure(const char* label, const char* expr, const char* file, int line, const char* func) {
    default_printer.fg = White;
    default_printer.bg = Red;
    dprint_str("\n");
    dprint_str(label);
    dprint_str(": ");
    dprint_str(expr);
    dprint_str(" failed\n");
    dprint_str(func);
    dprint_str(" (");
    dprint_str(file);
    dprint_str(":");
    dprint_dec(line);
    dprint_str(")\n");
}

void halt() {
    while (1) {
        asm("hlt");
    }
}

void __prefer_fail(const char* expr, const char* file, int line, const char* func) {
    print_failure("WARNING", expr, file, line, func);
}

void __assert_fail(const char* expr, const char* file, int line, const char* func) {
    print_failure("ERROR", expr, file, line, func);
    halt();
}

static void print_number_imp(
    printer* p,
    uint32_t num,
    int base,
    const char* symbols
) {
    if (num / base != 0) {
        print_number_imp(p, num / base, base, symbols);
    }
    print_char(p, symbols[num % base]);
}


void print_dec(printer* p, uint32_t num) {
    print_number_imp(p, num, 10, "0123456789");
}

void print_hex(printer* p, uint32_t num) {
    print_str(p, "0x");
    print_number_imp(p, num, 16, "0123456789abcdef");
}

void dprint_clear() {
    print_clear(&default_printer);
}
void dprint_str(const char* s) {
    print_str(&default_printer, s);
}
void dprint_dec(uint32_t v) {
    print_dec(&default_printer, v);
}
void dprint_hex(uint32_t v) {
    print_hex(&default_printer, v);
}
