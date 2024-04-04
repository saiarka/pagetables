#include "tlb.h"

struct tlb tlb = {
    .tlb_cache = {
        [0 ... 15] = {
            .way_array = {
                [0 ... 3] = {
                .valid_bit = 0,
                .tag_bits = 0,
                .lru_tracker = 0,
                .page_table_entry= 0
                }
            }
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
size_t page_table_entry_from_tlb;
size_t valid_address;

for(int i = 0; i < 4; i += 1) {
    if(set_from_index.way_array[i].tag_bits == va_tag_bits) {
        page_table_entry_from_tlb = set_from_index.way_array[i].page_table_entry;
        valid_address = valid_check(page_table_entry_from_tlb, offset_bits);
        return valid_address + offset_bits;
    }
}


}



