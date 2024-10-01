#include <stdint.h>
#include <kernel/atag.h>
#include <kernel/list.h>
#ifndef MEM_H
#define MEM_H

#define PAGE_SIZE 4096

// Metadata for the pages to keep track of them.
typedef struct {
    uint8_t allocated: 1;
    uint8_t kernel_page: 1;
    uint32_t reserved: 30;
} page_flags_t;

// More metadata to keep track of virtual memory locations.
typedef struct page {
    uint32_t vaddr_mapped;
    page_flags_t flags;
    DEFINE_LINK(page);
} page_t;

void mem_init(atag_t* atags);
void* alloc_page(void);
void free_page(void* ptr);

#endif
