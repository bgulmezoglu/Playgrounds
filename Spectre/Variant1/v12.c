#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>
// SPECTRE 12

void victim_function(size_t x, size_t y) {
    if ((x + y) < bufferSize)
        temp &= array[buffer[x + y] * 4096];
}