#pragma once
#include "disk/disk.h"

#include <stdint.h>

struct istream_vtable_t;
typedef struct istream_vtable_t istream_vtable;

typedef struct istream_t {
    const istream_vtable* vtable;
} istream;

// Create a stream reading sectors from the disk
// Reads to this stream must be multiples of the sector size.
istream* istream_raw_disk(const disk* disk, uint32_t start_sector);

// Create a stream which buffers reads to an
// underlying stream.  Reads can be arbitary
// sizes.
istream* istream_buffer(istream* delegate);

// Reads a certain number of bytes.  Errors
// if the full number of bytes was not available.
void istream_read(istream*, uint32_t length, void* output);

// Like istream_read, but returns the number of
// bytes read (which is only different at EOF).
uint32_t istream_read_partial(istream*, uint32_t length, void* output);

// Close/release/free a stream.
void istream_free(istream*);

