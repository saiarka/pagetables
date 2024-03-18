#include <stdio.h>
#include "config.h"
#include "mlpt.h"
#include <stdalign.h>
#include <assert.h>
#include <stdlib.h>

int main(){

assert(ptbr == 0);

    page_allocate(0x456789abcdef);

    printf("%p\n", (void*) ptbr);
    page_allocate(0x456789abcd00);

    int *p1 = (int *)translate(0x456789abcd00);
    *p1 = 0xaabbccdd;
    short *p2 = (short *)translate(0x456789abcd02);
    printf("%04hx\n", *p2); // prints "aabb\n"

    assert(translate(0x456789ab0000) == 0xFFFFFFFFFFFFFFFF);

    page_allocate(0x456789ab0000);
    
    assert(translate(0x456789ab0000) != 0xFFFFFFFFFFFFFFFF);
    
    page_allocate(0x456780000000);



}