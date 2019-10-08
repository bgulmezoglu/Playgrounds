#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>
// SPECTRE 6

void victim_function(size_t x) {
    if ((x & bufferSizeMask) == x)
        temp &= array[buffer[x] * 4096];
}