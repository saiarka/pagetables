#include "tlb.h"

struct way way_instance = {
    .valid_bit = 0,
    .tag_bits = 0,
    .lru_tracker_bits = 0,
    .phys_address = 0
};

struct cache_set cache_set_instance = {
    .way_array = {[0 ... 3] = way_instance},
    .lru_replacement_index = 0
};

// for(int i = 0; i < 4; i += 1) {
//     cache_set_instance.way_array[i] = way_instance;
// }

struct tlb tlb_return;
for(int i = 0; i < 16; i += 1){
    tlb_return.tlb_cache[i] = cache_set_instance;
}