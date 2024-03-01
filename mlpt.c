#include <stdio.h>
#include "mlpt.h"
#include "config.h"
#include <math.h>


size_t translate(size_t va){
size_t ptbr = ptbr_setup;
size_t translated_address;
size_t page_size = 1 << POBITS;
size_t num_of_entries = page_size / sizeof(size_t);
size_t offset_mask = (0x1 << POBITS) - 1;
size_t offset_bits = va & offset_mask;
size_t vpn_seg_bits = log2(num_of_entries);
size_t page_table_entry;
size_t *address_pointer;

va = va >> POBITS;
size_t vpn_mask;
size_t vpn_seg;    
if (LEVELS > 1)
{
    for (int i = 0; i < LEVELS; i += 1)
    {
        vpn_mask = (0x1 << vpn_seg_bits) - 1;
        vpn_seg = va & vpn_mask;
        ptbr = (vpn_seg * sizeof(size_t)) + ptbr;
        address_pointer = (size_t *) ptbr;
        page_table_entry = *address_pointer;
        ptbr = valid_check(page_table_entry, offset_bits);
        va = va >> vpn_seg_bits;
    }
    address_pointer = address_pointer + offset_bits;
    translated_address = *address_pointer;
}else {
    vpn_mask = (0x1 << vpn_seg_bits) - 1;
    vpn_seg = va & vpn_mask;
    ptbr = (vpn_seg * sizeof(size_t)) + ptbr;
    address_pointer = (size_t *) ptbr;
    page_table_entry = *address_pointer;
    translated_address = valid_check(page_table_entry, offset_bits);
}   
return translated_address;
}