#pragma once
#include <stdint.h>


int disk_read_sector(uint32_t start, uint32_t sector_count, void* output_buffer);