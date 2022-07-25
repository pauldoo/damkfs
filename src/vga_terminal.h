#pragma once

#include <stdint.h>

extern const int terminal_width;
extern const int terminal_height;

enum terminal_color {
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
};

void terminal_clear();

void terminal_put_char(int x, int y, char c, enum terminal_color fg, enum terminal_color bg);

void terminal_put_string(int x, int y, const char* s, enum terminal_color fg, enum terminal_color bg);

void terminal_print(const char* s, enum terminal_color fg, enum terminal_color bg);
