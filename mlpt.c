#define _XOPEN_SOURCE 700
#include <stdio.h>
#include "mlpt.h"
#include "config.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

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
    size_t vpn_segments[LEVELS];
    for (int i = (LEVELS - 1); i >= 0; i -= 1) 
    {
    vpn_mask = (0x1 << vpn_seg_bits) - 1;
    vpn_seg = va & vpn_mask;
    vpn_segments[i] = vpn_seg;
    va = va >> vpn_seg_bits;
    }

    for (int i = 0; i < LEVELS; i += 1)
    {
        vpn_seg = vpn_segments[i];
        ptbr_copy = (vpn_seg * sizeof(size_t)) + ptbr_copy;
        address_pointer = (size_t *) ptbr_copy;
        page_table_entry = *address_pointer;
        ptbr_copy = valid_check(page_table_entry, offset_bits);
        if (ptbr_copy == 0xFFFFFFFFFFFFFFFF) 
        {
            return 0xFFFFFFFFFFFFFFFF;
        }
    }
     ptbr_copy = ptbr_copy + offset_bits;
     translated_address = ptbr_copy;
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
size_t *ptbr_pointer = &ptbr;

size_t* pagetable_allocated_address;
size_t* multi_level_addresses[LEVELS];
size_t* datapage_allocated_address;

va = va >> POBITS;
size_t vpn_mask;
size_t vpn_seg;    
size_t vpn_segments[LEVELS];

if (ptbr_copy == 0 && LEVELS == 1)
{
    if (posix_memalign((void**)&pagetable_allocated_address, sizeof(size_t) * num_of_entries, sizeof(size_t) * num_of_entries) != 0) 
    {
        perror("Error with initial page entry allocated");
    }
    if (posix_memalign((void**)&datapage_allocated_address, sizeof(size_t) * num_of_entries, sizeof(size_t) * num_of_entries) != 0) 
    {
        perror("Error with initial data page allocated");
    }
    vpn_mask = (0x1 << vpn_seg_bits) - 1;
    vpn_seg = va & vpn_mask;
    *ptbr_pointer =(size_t)pagetable_allocated_address;
    ptbr_copy = ptbr;
    size_t address_for_data = (size_t)datapage_allocated_address;
    size_t phys_page_number_for_data = address_for_data / (sizeof(size_t) * num_of_entries);
    size_t page_table_entry_for_page = (phys_page_number_for_data << POBITS) | 1; 
    ptbr_copy = (vpn_seg * sizeof(size_t)) + ptbr_copy;
    address_pointer = (size_t *) ptbr_copy;
    *address_pointer = page_table_entry_for_page;

}else if (ptbr_copy == 0 && LEVELS > 1) 
{
    if (posix_memalign((void**)&multi_level_addresses[0], sizeof(size_t) * num_of_entries, sizeof(size_t) * num_of_entries) != 0) 
    {
        perror("Error with page entry allocated");
    }
    size_t level_one_address = (size_t)multi_level_addresses[0];
    ptbr_copy = (size_t)multi_level_addresses[0];
    
    for (int i = (LEVELS - 1); i >= 0; i -= 1) 
    {
    vpn_mask = (0x1 << vpn_seg_bits) - 1;
    vpn_seg = va & vpn_mask;
    vpn_segments[i] = vpn_seg;
    va = va >> vpn_seg_bits;
    }

    for (int i = 0; i < LEVELS; i += 1)
    {
        vpn_seg = vpn_segments[i];
        ptbr_copy = (vpn_seg * sizeof(size_t)) + ptbr_copy;
        address_pointer = (size_t *) ptbr_copy;
        page_table_entry = *address_pointer;
        ptbr_copy = valid_check(page_table_entry, offset_bits); 
        if (posix_memalign((void**)&multi_level_addresses[i+1], sizeof(size_t) * num_of_entries, sizeof(size_t) * num_of_entries) != 0) 
        {
            perror("Error with page entry allocated");
        }
            if (i != (LEVELS)) 
            {
                size_t address_for_next_level = (size_t)multi_level_addresses[i+1];
                size_t phys_page_number_for_level = address_for_next_level / (sizeof(size_t) * num_of_entries);
                size_t page_table_entry_for_next_level_page = (phys_page_number_for_level << POBITS) | 1;
                *address_pointer = page_table_entry_for_next_level_page;
                ptbr_copy = (size_t)multi_level_addresses[i+1];
            }
    }
    if (posix_memalign((void**)&datapage_allocated_address, sizeof(size_t) * num_of_entries, sizeof(size_t) * num_of_entries) != 0) 
    {
        perror("Error with data page allocated for missing entry");
    }
    size_t address_for_data = (size_t)datapage_allocated_address;
    size_t phys_page_number_for_data = address_for_data / (sizeof(size_t) * num_of_entries);
    size_t page_table_entry_for_page = (phys_page_number_for_data << POBITS) | 1;
    *address_pointer = page_table_entry_for_page;
    *ptbr_pointer = level_one_address;
} else 
{
    if (LEVELS > 1)
    {
        for (int i = (LEVELS - 1); i >= 0; i -= 1) 
        {
        vpn_mask = (0x1 << vpn_seg_bits) - 1;
        vpn_seg = va & vpn_mask;
        vpn_segments[i] = vpn_seg;
        va = va >> vpn_seg_bits;
        }
        for (int i = 0; i < LEVELS; i += 1)
        {
            vpn_seg = vpn_segments[i];
            ptbr_copy = (vpn_seg * sizeof(size_t)) + ptbr_copy;
            address_pointer = (size_t *) ptbr_copy;
            page_table_entry = *address_pointer;
            ptbr_copy = valid_check(page_table_entry, offset_bits); 
                //add implementation here for when new pagetable is needed b/c va exceeds page table entries allocated??
            if (ptbr_copy == 0xFFFFFFFFFFFFFFFF && i != (LEVELS - 1)) 
            {
                if (posix_memalign((void**)&pagetable_allocated_address, sizeof(size_t) * num_of_entries, sizeof(size_t) * num_of_entries) != 0) {
                    perror("Error with initial page entry allocated");
                }
                size_t address_for_next_level = (size_t)pagetable_allocated_address;
                size_t phys_page_number_for_level = address_for_next_level / (sizeof(size_t) * num_of_entries);
                size_t page_table_entry_for_next_level_page = (phys_page_number_for_level << POBITS) | 1;
                *address_pointer = page_table_entry_for_next_level_page;
                ptbr_copy = (size_t)pagetable_allocated_address;
            }
        }
        if (ptbr_copy == 0xFFFFFFFFFFFFFFFF) 
        {
            if (posix_memalign((void**)&datapage_allocated_address, sizeof(size_t) * num_of_entries, sizeof(size_t) * num_of_entries) != 0) 
            {
                perror("Error with data page allocated for missing entry");
            }
            size_t address_for_data = (size_t)datapage_allocated_address;
            size_t phys_page_number_for_data = address_for_data / (sizeof(size_t) * num_of_entries);
            size_t page_table_entry_for_page_vpnseg = (phys_page_number_for_data << POBITS) | 1;
            *address_pointer = page_table_entry_for_page_vpnseg;
        }
    }else {
        vpn_mask = (0x1 << vpn_seg_bits) - 1;
        vpn_seg = va & vpn_mask;
        ptbr_copy = (vpn_seg * sizeof(size_t)) + ptbr_copy;
        address_pointer = (size_t *) ptbr_copy;
        page_table_entry = *address_pointer;
        translated_address = valid_check(page_table_entry, offset_bits);
        if (translated_address == 0xFFFFFFFFFFFFFFFF) 
        {
            if (posix_memalign((void**)&datapage_allocated_address, sizeof(size_t) * num_of_entries, sizeof(size_t) * num_of_entries) != 0) 
            {
                perror("Error with data page allocated for missing entry");
            }

            size_t address_for_data = (size_t)datapage_allocated_address;
            size_t phys_page_number_for_data = address_for_data / (sizeof(size_t) * num_of_entries);
            size_t page_table_entry_for_page_vpnseg = (phys_page_number_for_data << POBITS) | 1;
            *address_pointer = page_table_entry_for_page_vpnseg;
            
        } 
    }   
}
}
