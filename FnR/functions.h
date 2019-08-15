#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "stubs.h"



void l1_init_LL(uint64_t * _base, uint8_t _set){
    for(int i = 0; i < CPU_L1_CACHE_SET_ASSOC - 1; i++)
        *(_base + l1_index(i, _set)) = (uint64_t)(_base + l1_index(i + 1, _set));
    for(int i = CPU_L1_CACHE_SET_ASSOC + 1; i > 0; i--)
        *(_base + l1_index(i, _set) + 1) = (uint64_t)(_base + l1_index(i - 1, _set) + 1);
}


uint64_t get_phys_addr(void * virtual_addr_p) {
    int fd = open("/proc/self/pagemap", O_RDONLY);
    assert(fd >= 0);
    maccess(virtual_addr_p);
    uint64_t virtual_addr = (uint64_t)virtual_addr_p;
    uint64_t value = 0;
    uint64_t offset = (virtual_addr / 4096) * sizeof(value);
    int got = pread(fd, &value, sizeof(value), offset);
    uint64_t deneme = value << 12;
    assert((value << 12) > 0);
    return (value << 12) | ((size_t)virtual_addr_p & 0xFFFULL);
}
