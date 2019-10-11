#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>

// SPECTRE 15

void victim_function(size_t *x) {
    if (*x < bufferSize)
        temp &= array[buffer[*x] * 4096];
}