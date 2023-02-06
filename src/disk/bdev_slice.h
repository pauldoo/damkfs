#pragma once

#include "disk/bdev.h"
#include "util/list.h"

// subset of blocks from an existing bdev, presented as another bdev
// takes ownership of the block list, but not the bdev.
bdev* bdev_slice_create(bdev* bdev, list* blocks, uint32_t logical_length);
