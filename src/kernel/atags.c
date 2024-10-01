#include <kernel/atag.h>


uint32_t get_mem_size(atag_t* tag) {
    // Iterating over the atag list
    while (tag->tag != NONE) {
        // If the tag is a memory tag, then we've found it
        if (tag->tag == MEM) {
            return tag->mem.size;
        }
        // Otherwise, get the pointer number to the current tag and add the total tag's size to it to move to the next tag.
        tag = ((uint32_t*)tag) + tag->tag_size;
    }

    // If none of that works, return the memory necessary.
    return 1024 * 1024 * 1024;
}
