#include "stream.h"
#include "disk/disk.h"
#include "terminal/terminal.h"
#include "memory/memory.h"

#define ISTREAM_BUFFER_LENGTH (4096)

typedef void (*StreamFree)(istream*);
typedef uint32_t (*StreamRead)(istream*, uint32_t, void*);
typedef void (*StreamSeek)(istream*, uint32_t);

struct istream_vtable_t {
    StreamFree free;
    StreamRead read_partial;
    StreamSeek seek;
    uint32_t seek_granularity;
};

typedef struct istreambdev_t {
    istream base;
    bdev* dev;
    uint32_t next_block;
} istreambdev;

typedef struct istreambuffer_t {
    istream base;
    istream* delegate;
    int next;
    int end;
    uint8_t buffer[ISTREAM_BUFFER_LENGTH];
} istreambuffer;

static void istream_bdev_free(istream* stream);
static uint32_t istream_bdev_read_partial(istream* stream, uint32_t length, void* output);
static void istream_bdev_seek(istream* stream, uint32_t offset);

static void istream_buffer_free(istream* stream);
static uint32_t istream_buffer_read_partial(istream* stream, uint32_t length, void* output);
static void istream_buffer_seek(istream* stream, uint32_t offset);

const istream_vtable istream_bdev_vtable = {
    .free = istream_bdev_free,
    .read_partial = istream_bdev_read_partial,
    .seek = istream_bdev_seek,
    .seek_granularity = 4096
};

const istream_vtable istream_buffer_vtable = {
    .free = istream_buffer_free,
    .read_partial = istream_buffer_read_partial,
    .seek = istream_buffer_seek,
    .seek_granularity = 1
};

static void istream_bdev_free(istream* stream) {
    ASSERT(stream != 0);
    ASSERT(stream->vtable == &istream_bdev_vtable);
    kfree(stream);
};

static uint32_t istream_bdev_read_partial(istream* stream, uint32_t length, void* output) {
    ASSERT(stream != 0);
    ASSERT(stream->vtable == &istream_bdev_vtable);
    istreambdev* db = (istreambdev*)stream;
    ASSERT(length % (db->dev->block_size) == 0);
    int blocks_to_read = length / (db->dev->block_size);
    bdev_read(db->dev, db->next_block, blocks_to_read, output);
    db->next_block += blocks_to_read;

    return length; // TK: EOF disk?
}

static void istream_bdev_seek(istream* stream, uint32_t offset) {
    ASSERT(stream != 0);
    ASSERT(stream->vtable == &istream_bdev_vtable);
    istreambdev* db = (istreambdev*)stream;
    ASSERT(offset % db->dev->block_size == 0);
    db->next_block = offset / db->dev->block_size;
}

istream* istream_bdev(bdev* dev) {
    istreambdev* result = kcalloc(sizeof(istreambdev));
    result->dev = dev;
    result->next_block = 0;
    result->base.vtable = &istream_bdev_vtable;
    ASSERT( (void*)&(result->base) == (void*)result );
    return &(result->base);
}

static void istream_buffer_free(istream* stream) {
    ASSERT(stream != 0);
    ASSERT(stream->vtable == &istream_buffer_vtable);
    istreambuffer* sb = (istreambuffer*)stream;
    istream_free(sb->delegate);
    kfree(sb);
}

static uint32_t istream_buffer_read_partial(istream* stream, const uint32_t length, void* const output) {
    ASSERT(stream != 0);
    ASSERT(stream->vtable == &istream_buffer_vtable);
    istreambuffer* const sb = (istreambuffer*)stream;

    uint32_t bytes_read = 0;
    while (bytes_read < length) {
        if (sb->next >= sb->end) {
            // Refill buffer
            sb->next = 0;
            sb->end = istream_read_partial(sb->delegate, ISTREAM_BUFFER_LENGTH, sb->buffer);
            if (sb->end == 0) {
                // EOF
                break;
            }
        }

        while ((bytes_read < length) && (sb->next < sb->end)) {
            ((uint8_t*)output)[bytes_read] = sb->buffer[sb->next];
            bytes_read += 1;
            sb->next += 1;
        }
    }

    return bytes_read;
}

static void istream_buffer_seek(istream* stream, uint32_t offset) {
    ASSERT(stream != 0);
    ASSERT(stream->vtable == &istream_buffer_vtable);
    istreambuffer* const sb = (istreambuffer*)stream;

    sb->next = 0;
    sb->end = 0;

    uint32_t fragment_length = offset % sb->delegate->vtable->seek_granularity;
    istream_seek(sb->delegate, offset - fragment_length);
    uint8_t discard_buffer[128] = { 0 };

    while (fragment_length != 0) {
        const uint32_t read_count = (fragment_length > 128) ? 128 : fragment_length;
        istream_read(stream, read_count, discard_buffer);
        fragment_length -= read_count;
    }
}

istream* istream_buffer(istream* delegate) {
    ASSERT(delegate != 0);
    istreambuffer* result = kcalloc(sizeof(istreambuffer));
    result->delegate = delegate;
    result->next = 0;
    result->end = 0;
    result->base.vtable = &istream_buffer_vtable;
    ASSERT( (void*)&(result->base) == (void*)result );
    return &(result->base);
}

uint32_t istream_read_partial(istream* stream, uint32_t length, void* output) {
    return stream->vtable->read_partial(stream, length, output);
}

void istream_read(istream* stream, uint32_t length, void* output) {
    uint32_t bytes_read = istream_read_partial(stream, length, output);
    ASSERT(bytes_read == length);
}

void istream_seek(istream* stream, uint32_t offset) {
    ASSERT(offset % stream->vtable->seek_granularity == 0);
    stream->vtable->seek(stream, offset);
}

void istream_free(istream* stream) {
    stream->vtable->free(stream);
}

