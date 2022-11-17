#include "stream.h"
#include "disk/disk.h"
#include "terminal/terminal.h"
#include "memory/memory.h"

#define ISTREAM_BUFFER_LENGTH (4096)

typedef void (*StreamFree)(istream*);
typedef uint32_t (*StreamRead)(istream*, uint32_t, void*);

struct istream_vtable_t {
    StreamFree free;
    StreamRead read_partial;
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

static void istream_buffer_free(istream* stream);
static uint32_t istream_buffer_read_partial(istream* stream, uint32_t length, void* output);

const istream_vtable raw_disk_vtable = {
    .free = istream_raw_disk_free,
    .read_partial = istream_raw_disk_read_partial
};

const istream_vtable buffer_vtable = {
    .free = istream_buffer_free,
    .read_partial = istream_buffer_read_partial
};

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

istream* istream_raw_disk(const disk* disk, uint32_t start_sector) {
    istreamdisk* result = kcalloc(sizeof(istreamdisk));
    result->disk = disk;
    result->next_sector = start_sector;
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

void istream_free(istream* stream) {
    stream->vtable->free(stream);
}

