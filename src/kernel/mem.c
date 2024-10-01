#include <kernel/mem.h>
#include <kernel/atag.h>
#include <common/stdlib.h>
#include <stdint.h>
#include <stddef.h>

// Pull in the __end variable
extern uint8_t __end;

// This variable is only accessible by this program
static uint32_t num_pages;

// Create a linked list of pages
DEFINE_LIST(page);
IMPLEMENT_LIST(page);

// This will hold an array of all the pages
static page_t* all_pages_array;

// This is the linked list of free pages
page_list_t free_pages;

// Initialize the memory
void mem_init(atag_t * atags) {
    uint32_t mem_size,  page_array_len, kernel_pages, i;

    // Get the total number of pages
    mem_size = get_mem_size(atags);
    num_pages = mem_size / PAGE_SIZE;

    // Allocate space for all those pages' metadata.  Start this block just after the kernel image is finished
    // The length of the page array is the number of pages times the size of each page
    page_array_len = sizeof(page_t) * num_pages;
    // The pages array starts at the address of the end of the kernel image
    all_pages_array = (page_t *)&__end;
    // Zero the entire page array section
    bzero(all_pages_array, page_array_len);

    // Initialize the list of free_pages
    INITIALIZE_LIST(free_pages);

    // Iterate over all pages and mark them with the appropriate flags
    // Start with kernel pages

    // The number of pages the kernel image uses
    kernel_pages = ((uint32_t)&__end) / PAGE_SIZE;
    for (i = 0; i < kernel_pages; i++) {
        all_pages_array[i].vaddr_mapped = i * PAGE_SIZE;    // Identity map the kernel pages
        all_pages_array[i].flags.allocated = 1;
        all_pages_array[i].flags.kernel_page = 1;
    }
    // Map the rest of the pages as unallocated, and add them to the free list
    for(; i < num_pages; i++){
        // Mark the rest as unallocated and append them to free pages linked list
        all_pages_array[i].flags.allocated = 0;
        append_page_list(&free_pages, &all_pages_array[i]);
    }

}


// Dynamic allocation and freeing
void * alloc_page(void) {
    page_t * page;
    void * page_mem;
    
    // If there aren't any more pages left we can't allocate anything
    if (size_page_list(&free_pages) == 0)
        return 0;

    // Get a free page
    // Pop a free page off the free page list
    page = pop_page_list(&free_pages);
    // Allocate it as a kernel page
    page->flags.kernel_page = 1;
    // Set it as allocated
    page->flags.allocated = 1;

    // Get the address the physical page metadata refers to
    // The location of the page in memory - the location of the all pages array start, all that times page size will give us the location of the 4KB block of page.
    page_mem = (void *)((page - all_pages_array) * PAGE_SIZE);

    // Zero out the page, big security flaw to not do this :)
    bzero(page_mem, PAGE_SIZE);

    return page_mem;
}

void free_page(void * ptr) {
    page_t * page;

    // Get page metadata from the physical address
    // At the address in the array of page metadata (not the actual page itself). The division tells us which index that page of data is in. Dividing by PAGE_SIZE yields how many 4KB blocks away in the pages away we are
    page = all_pages_array + ((uint32_t)ptr / PAGE_SIZE);

    // Mark the page as free
    page->flags.allocated = 0;

    // Add it to the free list
    append_page_list(&free_pages, page);
}
