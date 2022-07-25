#pragma once

#include <stdint.h>

uint8_t in_b(uint16_t port);
uint16_t in_w(uint16_t port);

void out_b(uint16_t port, uint8_t value);
void out_w(uint16_t port, uint16_t value);
