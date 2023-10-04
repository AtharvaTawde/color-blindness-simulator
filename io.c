#include "io.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct buffer {
    int fd;
    int offset;
    int num_remaining;
    uint8_t a[BUFFER_SIZE];
};

Buffer *read_open(const char *filename) {
    int f = open(filename, O_RDONLY);

    if (f < 0)
        return NULL;

    Buffer *b = malloc(sizeof(Buffer));
    b->fd = f;
    b->num_remaining = 0;
    b->offset = 0;

    return b;
}

void read_close(Buffer **pbuf) {
    close((*pbuf)->fd);
    free(*pbuf);
    *pbuf = NULL;
}

bool read_uint8(Buffer *buf, uint8_t *x) {
    if (buf->num_remaining == 0) {
        ssize_t rc = read(buf->fd, buf->a, sizeof(buf->a));
        if (rc < 0)
            fprintf(stderr, "Error reading this byte.\n");
        if (rc == 0)
            return false;
        buf->num_remaining = rc;
        buf->offset = 0;
    }

    *x = buf->a[buf->offset];
    buf->offset += sizeof(uint8_t);
    buf->num_remaining -= sizeof(uint8_t);
    return true;
}

bool read_uint16(Buffer *buf, uint16_t *x) {
    uint8_t b1, b2;

    if (!read_uint8(buf, &b1) || !read_uint8(buf, &b2))
        return false;

    *x = b2 << 8;
    *x |= b1;

    return true;
}

bool read_uint32(Buffer *buf, uint32_t *x) {
    uint16_t b1, b2;

    if (!read_uint16(buf, &b1) || !read_uint16(buf, &b2))
        return false;

    *x = b2 << 16;
    *x |= b1;

    return true;
}

Buffer *write_open(const char *filename) {
    int f = creat(filename, 0664);

    if (f < 0)
        return NULL;

    Buffer *b = malloc(sizeof(Buffer));
    b->fd = f;
    b->num_remaining = 0;
    b->offset = 0;
    return b;
}

void write_close(Buffer **pbuf) {
    uint8_t *start = (*pbuf)->a;
    int num_bytes = (*pbuf)->offset;

    do {
        ssize_t rc = write((*pbuf)->fd, start, num_bytes);
        if (rc < 0)
            fprintf(stderr, "Error writing this byte.\n");
        start += rc;
        num_bytes -= rc;
    } while (num_bytes > 0);

    (*pbuf)->offset = 0;

    close((*pbuf)->fd);
    free(*pbuf);
    *pbuf = NULL;
}

void write_uint8(Buffer *buf, uint8_t x) {
    if (buf->offset == BUFFER_SIZE) {
        uint8_t *start = buf->a;
        int num_bytes = buf->offset;

        do {
            ssize_t rc = write(buf->fd, start, num_bytes);
            if (rc < 0)
                fprintf(stderr, "Error writing this byte.\n");
            start += rc;
            num_bytes -= rc;
        } while (num_bytes > 0);

        buf->offset = 0;
    }

    buf->a[buf->offset] = x;
    buf->offset += sizeof(uint8_t);
}

void write_uint16(Buffer *buf, uint16_t x) {
    write_uint8(buf, (uint8_t) (x & 0xff));
    write_uint8(buf, (uint8_t) ((x >> 8) & 0xff));
}

void write_uint32(Buffer *buf, uint32_t x) {
    write_uint16(buf, (uint16_t) (x & 0xffff));
    write_uint16(buf, (uint16_t) ((x >> 16) & 0xffff));
}
