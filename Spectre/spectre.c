#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
int main(){

    printf("Spectre is started\n");


    uint64_t * probe_array = (uint64_t *) memalign(4096, 256 * 512 * sizeof(uint64_t));
    for (int i = 0; i < 512 * 256; i++)
    {
        probe_array[i] = i;

    }
    
    clflush_element(probe_array);
    clflush_array(probe_array, 512 * 256);
    return 0;
}