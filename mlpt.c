#define _XOPEN_SOURCE 700
#include <stdio.h>
#include "mlpt.h"
#include "config.h"
#include <math.h>

size_t translate(size_t va){
size_t translated_address;
size_t page_size = 1 << POBITS;
size_t num_of_entries = page_size / sizeof(size_t);
size_t offset_mask = (0x1 << POBITS) - 1;
size_t offset_bits = va & offset_mask;
size_t vpn_seg_bits = log2(num_of_entries);
size_t page_table_entry;
size_t *address_pointer;
size_t ptbr_copy = ptbr;

if(ptbr_copy == 0){
return 0xFFFFFFFFFFFFFFFF;
}

va = va >> POBITS;
size_t vpn_mask;
size_t vpn_seg;    
if (LEVELS > 1)
{
    for (int i = 0; i < LEVELS; i += 1)
    {
        vpn_mask = (0x1 << vpn_seg_bits) - 1;
        vpn_seg = va & vpn_mask;
        ptbr_copy = (vpn_seg * sizeof(size_t)) + ptbr_copy;
        address_pointer = (size_t *) ptbr_copy;
        page_table_entry = *address_pointer;
        ptbr_copy = valid_check(page_table_entry, offset_bits);
        va = va >> vpn_seg_bits;
    }
    address_pointer = address_pointer + offset_bits;
    translated_address = *address_pointer;
}else {
    vpn_mask = (0x1 << vpn_seg_bits) - 1;
    vpn_seg = va & vpn_mask;
    ptbr_copy = (vpn_seg * sizeof(size_t)) + ptbr_copy;
    address_pointer = (size_t *) ptbr_copy;
    page_table_entry = *address_pointer;
    translated_address = valid_check(page_table_entry, offset_bits);
}   
return translated_address;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void page_allocate(size_t va){
size_t translated_address;
size_t page_size = 1 << POBITS;
size_t num_of_entries = page_size / sizeof(size_t);
size_t offset_mask = (0x1 << POBITS) - 1;
size_t offset_bits = va & offset_mask;
size_t vpn_seg_bits = log2(num_of_entries);
size_t page_table_entry;
size_t *address_pointer;
size_t ptbr_copy = ptbr;
size_t *ptbr_pointer = ptbr;

size_t* pagetable_allocated_address;
size_t* datapage_allocated_address;

if (ptbr_copy == 0 && LEVELS == 1)
{
            //Potential idea --> set ptbr to certain area in allocated page table
            //Also have to allocate memory for a 'data' page for some bs idk
            //Have to account for the fact that the first call makes two allocations, and subsequent calls may only make allocations for one page of data
            //Check for validation for whether page number supplied to page_allocate is not marked valid in page table
    if (posix_memalign(&pagetable_allocated_address, sizeof(size_t) * num_of_entries, sizeof(size_t) * num_of_entries) != 0) 
    {
        perror("Error with page entry allocated");
    }
    if (posix_memalign(&datapage_allocated_address, sizeof(size_t) * num_of_entries, sizeof(size_t) * num_of_entries) != 0) 
    {
        perror("Error with data page allocated");
    }

    *ptbr_pointer = *pagetable_allocated_address;
    size_t address_for_data = *datapage_allocated_address;
    size_t phys_page_number_for_data = address_for_data / (sizeof(size_t) * num_of_entries);
    size_t page_table_entry_for_page_0 = (phys_page_number_for_data << POBITS) | 1;
    ptbr_pointer[0] = page_table_entry_for_page_0;

}else 
{
    va = va >> POBITS;
    size_t vpn_mask;
    size_t vpn_seg;    
    if (LEVELS > 1)
    {
        //!Have to implement method here to allocate pagetable when neccessary and data table when neccessary
        for (int i = 0; i < LEVELS; i += 1)
        {
            vpn_mask = (0x1 << vpn_seg_bits) - 1;
            vpn_seg = va & vpn_mask;
            ptbr_copy = (vpn_seg * sizeof(size_t)) + ptbr_copy;
            address_pointer = (size_t *) ptbr_copy;
            page_table_entry = *address_pointer;
            ptbr_copy = valid_check(page_table_entry, offset_bits); 
                if (ptbr_copy == 0xFFFFFFFFFFFFFFFF) 
                {


                }
            va = va >> vpn_seg_bits;
        }
        address_pointer = address_pointer + offset_bits;
    //    translated_address = *address_pointer;
    }else {
        vpn_mask = (0x1 << vpn_seg_bits) - 1;
        vpn_seg = va & vpn_mask;
        ptbr_copy = (vpn_seg * sizeof(size_t)) + ptbr_copy;
        address_pointer = (size_t *) ptbr_copy;
        page_table_entry = *address_pointer;
        translated_address = valid_check(page_table_entry, offset_bits);

        if (translated_address == 0xFFFFFFFFFFFFFFFF) 
        {
            if (posix_memalign(&datapage_allocated_address, sizeof(size_t) * num_of_entries, sizeof(size_t) * num_of_entries) != 0) 
            {
                perror("Error with data page allocated");
            }

            ptbr_copy = (vpn_seg * sizeof(size_t)) + ptbr_copy;
            address_pointer = (size_t *) ptbr_copy;

        }
    }   
}
}
