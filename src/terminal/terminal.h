#pragma once

#include <stdint.h>

extern const int terminal_width;
extern const int terminal_height;

typedef enum terminal_color_t {
 Black = 0,
 Blue,
 Green,
 Cyan,
 Red,
 Magenta,
 Brown,
 Light_Gray,
 Dark_Gray,
 Light_Blue,
 Light_Green,
 Light_Cyan,
 Light_Red,
 Light_Magenta,
 Yellow,
 White
} terminal_color;

typedef struct printer_t {
  int x;
  int y;
  terminal_color fg;
  terminal_color bg;
} printer;

// "default" printer (used by "dprint_*")
extern printer default_printer;

// Character by character access
void terminal_put_char(int x, int y, char c, terminal_color fg, terminal_color bg);

// Stateful "printer"
void print_clear(printer* p);
void print_char(printer* p, char c);
void print_str(printer* p, const char* s);
void print_dec(printer* p, uint32_t);
void print_hex(printer* p, uint32_t);

// Utils for default printer
void dprint_clear();
void dprint_str(const char* s);
void dprint_dec(uint32_t v);
void dprint_hex(uint32_t v);



void halt() __attribute__ ((noreturn));

void __prefer_fail(const char* expr, const char* file, int line, const char* func);

void __assert_fail(const char* expr, const char* file, int line, const char* func) __attribute__ ((noreturn));

#define PREFER(expr) if (!!(expr)) {} else { __prefer_fail(#expr, __FILE__, __LINE__, __func__); }

#define ASSERT(expr) if (!!(expr)) {} else { __assert_fail(#expr, __FILE__, __LINE__, __func__); }
