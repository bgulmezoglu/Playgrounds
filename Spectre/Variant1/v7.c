#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>
// SPECTRE V7

/****************************************
 * SPECTRE V7
 * Not working.
 * I assume that, flushing bufferSize don't do
 * anything to the comparison. That's why it won't
 * speculatively, or, do the speculation not fast enough.
 * **************************************/
/**
 * Update!
 * When we changed the function from void to size_t and
 * when we flush the static location, we can access to the array
 * speculatively.
 * However, the last 5-6 elements cannot be accessible.
 * TODO
 * Why? 
 */

size_t* victim_function(size_t x) {
    
    static size_t last_x = 0;
	if (x == last_x)
		temp &= array[buffer[x] * 4096];
	
	if (x < bufferSize)
		last_x = x;
    
    return &last_x;
}