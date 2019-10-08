#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>
// SPECTRE 5

// As we are accessing many elements in the for loop
// it is not working properly
// 
// When "break" is present, it works because only one element
// is accessed
void victim_function(size_t x){
    int i;
    if(x < bufferSize){
        for(i = x - 1; i >= 0; i--){
            temp &= array[buffer[i] * 4096];
            break;
        }
    }
    
}