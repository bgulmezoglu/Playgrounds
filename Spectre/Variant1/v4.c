#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>

// SPECTRE V4
// TODO
// Problem
void victim_function(size_t x) {
    if (x < bufferSize)
        temp &= array[buffer[x << 1] * 4096];
}
