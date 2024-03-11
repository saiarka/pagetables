#include <stdio.h>
#include "config.h"

size_t valid_check(size_t page_table_entry, size_t offset_bits){
size_t valid_check = page_table_entry & 0x1;
size_t return_address;
if (valid_check == 1)
{
    // size_t significant_bits_size = 64 - POBITS;
    // size_t significant_bits = 0x0;
    // for (size_t i = 0; i < significant_bits_size; i += 1)
    // {
        
    // }
    size_t phys_page_number = page_table_entry >> POBITS;
    return_address = phys_page_number << POBITS;

   if (LEVELS == 1) 
   {
        return_address = return_address + offset_bits;
   }

}else {
    return_address = 0xFFFFFFFFFFFFFFFF;
}
return return_address;
}
