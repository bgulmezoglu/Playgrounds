#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "stubs.h"



int main (int argc, char * argv[])
{
    
    if(argc < 3) {
        printf("Usage: %s <sample_count> <delay_count>\r\n", argv[0]);
        return 1;
    }

    int sample_count = atoi(argv[1]);
    int delay_loop_count = atoi(argv[2]);

    warmup();

    uint32_t * samples = (uint32_t *) malloc
        (sample_count * sizeof(uint32_t));

    
    for(int c = 0; c < sample_count; c++){
        flush_reload(main, &samples[c]);        
        
        for(int i = 0; i < delay_loop_count; i++);
    }

    uint64_t sum = 0;
    
    for(int c = 0; c < sample_count; c++ ){        
        uint32_t v = samples[c];
        sum += v;
        printf("%u,", v);               
    }
    printf("\naverage:\n");    
    printf("%lu,", sum / sample_count);      
    printf("\n");

}
