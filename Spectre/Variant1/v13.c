#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>

inline __attribute__((always_inline)) int is_x_safe(size_t x) { if (x < bufferSize) return 1; return 0; }
void victim_function(size_t x) {
     if (is_x_safe(x))
          temp &= array[buffer[x] * 4096];
}