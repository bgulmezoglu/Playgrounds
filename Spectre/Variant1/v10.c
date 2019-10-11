#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <unistd.h> 
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>
#include <setjmp.h>

// SIGNAL HANDLERS
static jmp_buf trycatch_buf;


void unblock_signal(int signum __attribute__((__unused__))) {
  sigset_t sigs;
  sigemptyset(&sigs);
  sigaddset(&sigs, signum);
  sigprocmask(SIG_UNBLOCK, &sigs, NULL);
}

void trycatch_segfault_handler(int signum) {
  (void)signum;
  unblock_signal(SIGSEGV);
  unblock_signal(SIGFPE);
  longjmp(trycatch_buf, 1);
}


// SPECTRE 10
/**
 * Null pointer access and signal handler is added 
 * to improve accuracy.
 * Without them, there is no hit in the cache
*/ 
void victim_function(size_t x, uint8_t k) {
    signal(SIGSEGV, trycatch_segfault_handler);

    if (!setjmp(trycatch_buf)) {

        // null pointer access
        // it will raise an exception. During exception handling
        // below lines are executed speculatively.
        *(volatile char*)0;
        if (x < bufferSize) {
            if (buffer[x] == k){
                temp &= array[3 * 4096];
            }
        }
    }
}