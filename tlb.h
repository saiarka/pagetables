#include "config.h" /* see pagtable guidance on this file */
#include "mlpt.h"   /* see pagetable this file */

/** invalidate all cache lines in the TLB */
void tlb_clear();

/**
 * return 0 if this virtual address does not have a valid
 * mapping in the TLB. Otherwise, return its LRU status: 1
 * if it is the most-recently used, 2 if the next-to-most,
 * etc.
 */
int tlb_peek(size_t va);

/**
 * If this virtual address is in the TLB, return its
 * corresponding physical address. If not, use
 * `translate(va)` to find that address, store the result
 * in the TLB, and return it. In either case, make its
 * cache line the most-recently used in its set.
 *
 * As an exception, if translate(va) returns -1, do not
 * update the TLB: just return -1.
 */
size_t tlb_translate(size_t va);

struct way {
    size_t valid_bit;
    size_t tag_bits;
    int lru_tracker;
    size_t page_table_entry;
};

struct cache_set {
    struct way way_array[4];
    size_t lru_replacement_index;
};

struct tlb {
    struct cache_set tlb_cache[16];
};

extern struct way way_instance;
extern struct cache_set cache_set_instance;
extern struct tlb tlb;
