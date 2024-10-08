#include <stdint.h>

#ifndef ATAG_H
#define ATAG_H

// Specific atags
typedef enum {
    NONE = 0x00000000,
    CORE = 0x54410001,
    MEM = 0x54420003,
    INITRD2 = 0x54420005,
    CMDLINE = 0x54410009
} atag_tag_t;

typedef struct {
    uint32_t size;
    uint32_t start;
} mem_t;

typedef struct {
    uint32_t size;
    uint32_t start;
} initrd2_t;

typedef struct {
    uint32_t size;
    uint32_t start;
} cmdline_t;

// Define atag struct containing the tag size, the tag type, and a union that can store the three different tag types in a single location. Only of those tags will exist in that memory location, and the others will be zeroed out.
typedef struct atag {
    uint32_t tag_size;
    atag_tag_t tag;
    union {
        mem_t mem;
        initrd2_t initrd2;
        cmdline_t cmdline;
    };
} atag_t;

// For getting the memory size.
uint32_t get_mem_size(atag_t * tag);

#endif
