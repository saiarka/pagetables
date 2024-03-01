#include <stdio.h>
#include "config.h"
#include "mlpt.h"
#include <stdalign.h>

size_t ptbr;
alignas(4096) static size_t page_of_data[512];

static void set_testing_ptbr(void) {
    ptbr = (size_t) &page_of_data[0];
}

int main(){

set_testing_ptbr();
size_t translated = translate(0x0123456789abcdef);
printf("%zx\n", translated);
}