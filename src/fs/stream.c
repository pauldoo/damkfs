#include "stream.h"
#include "disk/disk.h"
#include "terminal/terminal.h"
#include "memory/memory.h"

#define ISTREAM_BUFFER_LENGTH (4096)

typedef void (*StreamFree)(istream*);
typedef uint32_t (*StreamRead)(istream*, uint32_t, void*);
typedef void (*StreamSeek)(istream*, uint64_t);

struct istream_vtable_t {
    StreamFree free;
    StreamRead read_partial;
    StreamSeek seek;
    uint32_t seek_granularity;
};

typedef struct istreamdisk_t {
    istream base;
    const disk* disk;
    uint32_t next_sector;
} istreamdisk;

typedef struct istreambuffer_t {
    istream base;
    istream* delegate;
    int next;
    int end;
    uint8_t buffer[ISTREAM_BUFFER_LENGTH];
} istreambuffer;

static void istream_raw_disk_free(istream* stream);
static uint32_t istream_raw_disk_read_partial(istream* stream, uint32_t length, void* output);
static void istream_raw_disk_seek(istream* stream, uint64_t offset);

static void istream_buffer_free(istream* stream);
static uint32_t istream_buffer_read_partial(istream* stream, uint32_t length, void* output);
static void istream_buffer_seek(istream* stream, uint64_t offset);

const istream_vtable raw_disk_vtable = {
    .free = istream_raw_disk_free,
    .read_partial = istream_raw_disk_read_partial,
    .seek = istream_raw_disk_seek,
    .seek_granularity = 4096
};

const istream_vtable buffer_vtable = {
    .free = istream_buffer_free,
    .read_partial = istream_buffer_read_partial,
    .seek = istream_buffer_seek,
    .seek_granularity = 1
};

static uint64_t u64mod(uint64_t x, uint32_t y) {
    ASSERT( y > 0 );
    ASSERT( (y & (y-1)) == 0 );
    return x & (y-1);
}

static uint64_t u64div(uint64_t x, uint32_t y) {
    ASSERT( y > 0 );
    ASSERT( (y & (y-1)) == 0 );
    int s = 0;
    while (y > 0) {
        s += 1;
        y /= 2;
    }
    return x >> s;
}

static void istream_raw_disk_free(istream* stream) {
    ASSERT(stream != 0);
    ASSERT(stream->vtable == &raw_disk_vtable);
    kfree(stream);
};

static uint32_t istream_raw_disk_read_partial(istream* stream, uint32_t length, void* output) {
    ASSERT(stream != 0);
    ASSERT(stream->vtable == &raw_disk_vtable);
    istreamdisk* db = (istreamdisk*)stream;
    ASSERT(length % (db->disk->sector_size) == 0);
    int sectors_to_read = length / (db->disk->sector_size);
    disk_read_sector(db->disk, db->next_sector, sectors_to_read, output);
    db->next_sector += sectors_to_read;

    return length; // TK: EOF disk?
}

static void istream_raw_disk_seek(istream* stream, uint64_t offset) {
    ASSERT(stream != 0);
    ASSERT(stream->vtable == &raw_disk_vtable);
    istreamdisk* db = (istreamdisk*)stream;
    ASSERT(u64mod(offset, db->disk->sector_size) == 0);
    db->next_sector = u64div(offset, db->disk->sector_size);
}

istream* istream_raw_disk(const disk* disk) {
    istreamdisk* result = kcalloc(sizeof(istreamdisk));
    result->disk = disk;
    result->next_sector = 0;
    result->base.vtable = &raw_disk_vtable;
    ASSERT( (void*)&(result->base) == (void*)result );
    return &(result->base);
}

static void istream_buffer_free(istream* stream) {
    ASSERT(stream != 0);
    ASSERT(stream->vtable == &buffer_vtable);
    istreambuffer* sb = (istreambuffer*)stream;
    istream_free(sb->delegate);
    kfree(sb);
}

static uint32_t istream_buffer_read_partial(istream* stream, const uint32_t length, void* const output) {
    ASSERT(stream != 0);
    ASSERT(stream->vtable == &buffer_vtable);
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

static void istream_buffer_seek(istream* stream, uint64_t offset) {
    ASSERT(stream != 0);
    ASSERT(stream->vtable == &buffer_vtable);
    istreambuffer* const sb = (istreambuffer*)stream;

    sb->next = 0;
    sb->end = 0;

    uint32_t fragment_length = u64mod(offset, sb->delegate->vtable->seek_granularity);
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
    result->base.vtable = &buffer_vtable;
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

void istream_seek(istream* stream, uint64_t offset) {
    ASSERT(u64mod(offset, stream->vtable->seek_granularity) == 0);
    stream->vtable->seek(stream, offset);
}

void istream_free(istream* stream) {
    stream->vtable->free(stream);
}

