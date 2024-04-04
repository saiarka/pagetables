#include "tlb.h"

struct tlb tlb = {
    .tlb_cache = {
        [0 ... 15] = {
            .way_array = {
                [0 ... 3] = {
                .valid_bit = 0,
                .tag_bits = 0,
                .lru_tracker = 0,
                .physical_address= 0
                }
            },
            .is_full = 0,
            .lru_replacement_index = 0,
        }
    }
};

void tlb_clear() {
   for(int i = 0; i < 16; ++i) {
    for(int j = 0; j < 4; ++i) {
        tlb.tlb_cache[i].way_array[j].valid_bit = 0;
    }
   }
}

int tlb_peek(size_t va) {
size_t usable_address = va >> POBITS;
size_t index_from_address = usable_address & 0xF;
struct cache_set set_from_index = tlb.tlb_cache[index_from_address];
size_t va_tag_bits = usable_address >> 1; 

if(translate(va) == -1) {
    return -1;
}

for(int i = 0; i < 4; i += 1) {
    if(set_from_index.way_array[i].tag_bits == va_tag_bits) {
        return set_from_index.way_array[i].lru_tracker;
    }
}

return 0;
}

size_t tlb_translate(size_t va) {
size_t usable_address = va >> POBITS;
size_t offset_bits = va & ((0x1 << POBITS) - 1);
size_t index_from_address = usable_address & 0xF;
struct cache_set set_from_index = tlb.tlb_cache[index_from_address];
size_t va_tag_bits = usable_address >> 1; 
size_t address_to_return;

for(int i = 0; i < 4; i += 1) {
    if(set_from_index.way_array[i].tag_bits == va_tag_bits) {
        return set_from_index.way_array[i].physical_address;
    }
}

if (set_from_index.is_full == 1) {
    for(int i = 0; i < 4; i += 1) {
        if(set_from_index.way_array[i].lru_tracker == 4) {
            set_from_index.way_array[i].tag_bits = va_tag_bits;
            set_from_index.way_array[i].physical_address= translate(va);
            address_to_return = set_from_index.way_array[i].physical_address;
            set_from_index.way_array[i].valid_bit = 1;
            set_from_index.way_array[i].lru_tracker = 1;
        }else {
            set_from_index.way_array[i].lru_tracker += 1;
        }
    }
}else {
    for(int i = 0; i < 4; i += 1) {
        if(set_from_index.way_array[i].valid_bit == 1) {
            set_from_index.way_array[i].lru_tracker += 1;
        }
    }
    for(int i = 0; i < 4; i += 1) {
        if(set_from_index.way_array[i].valid_bit == 0) {
            set_from_index.way_array[i].tag_bits = va_tag_bits;
            set_from_index.way_array[i].physical_address= translate(va);
            address_to_return = set_from_index.way_array[i].physical_address;
            set_from_index.way_array[i].valid_bit = 1;
            set_from_index.way_array[i].lru_tracker = 1;
        }
    }
}

return address_to_return;
}



