#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>

// SPECTRE 3:  Moving the leak to a function that cannot be inlined.
//
// Comments: Output is unsafe.  The same results occur if leakByteNoinlineFunction() 
// is in another source module.

__attribute_noinline__ void leakByteNoinlineFunction(uint8_t k) { temp &= array[(k)* 4096]; }
void victim_function(size_t x) {
    if (x < bufferSize)
    leakByteNoinlineFunction(buffer[x]);
}
