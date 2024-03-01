#include <stdio.h>
#include "config.h"
#include "mlpt.h"
#include <stdalign.h>
size_t ptbr;
alignas(4096)
static size_t page_of_data[512];
static void set_testing_ptbr(void) {
    ptbr = (size_t) &page_of_data[0];
}

alignas(4096)
static char data_for_page_3[4096];

int main(){


size_t address_of_data_for_page_3_as_integer = (size_t) &data_for_page_3[0]; 
size_t physical_page_number_of_data_for_page_3 = address_of_data_for_page_3_as_integer >> 12;
    // instead of >> 12, we could have written:
        // address_of_data_for_page_3_as_integer / 4096
size_t page_table_entry_for_page_3 = (
        // physical page number in upper (64-POBITS) bits
        (physical_page_number_of_data_for_page_3 << 12)
    |
        // valid bit in least significant bit, set to 1
        1
);

set_testing_ptbr();
// assuming testing_page_table initialized as above and ptbr points to it
page_of_data[3] = page_table_entry_for_page_3;
size_t translated = translate(0x3045);
printf("%zx\n", translated);
printf("%zx\n",(size_t) &data_for_page_3[0x45]);
if(translated == (size_t) &data_for_page_3[0x45]){
    printf("done");
}
}