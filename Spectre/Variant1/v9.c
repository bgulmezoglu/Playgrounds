#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>

// SPECTRE V9
/**
 * Last 5-6 characters are not leaking very well
 */ 
void victim_function(size_t x, int *x_is_safe) {
    if (*x_is_safe)
        temp &= array[buffer[x] * 4096];
}