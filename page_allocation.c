#define _XOPEN_SOURCE 700
#include <stdio.h>
#include "mlpt.h"
#include "config.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

size_t data_page_allocate_page_table_entry(size_t* data_page_allocated_address, size_t num_of_entries) {

    if (posix_memalign((void**)&data_page_allocated_address, sizeof(size_t) * num_of_entries, sizeof(size_t) * num_of_entries) != 0) {
        perror("Error with data page allocated for missing entry");
    }

    memset((void*)data_page_allocated_address, 0, sizeof(size_t) * num_of_entries);
    size_t address_for_data = (size_t)data_page_allocated_address;
    size_t phys_page_number_for_data = address_for_data / (sizeof(size_t) * num_of_entries);
    size_t page_table_entry_for_page = (phys_page_number_for_data << POBITS) | 1; 

  return page_table_entry_for_page;
}

size_t page_allocate_page_table_entry(size_t* page_allocated_address, size_t num_of_entries) {

    if (posix_memalign((void**)&page_allocated_address, sizeof(size_t) * num_of_entries, sizeof(size_t) * num_of_entries) != 0) {
        perror("Error with initial page entry allocated");
    }
    memset((void*)page_allocated_address, 0, sizeof(size_t) * num_of_entries);
    size_t address_for_next_level = (size_t)page_allocated_address;
    size_t phys_page_number_for_level = address_for_next_level / (sizeof(size_t) * num_of_entries);
    size_t page_table_entry_for_page = (phys_page_number_for_level << POBITS) | 1;

  return page_table_entry_for_page;
}


