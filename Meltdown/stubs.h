#define CPU_L1_CACHE_SET_COUNT   64
#define CPU_L1_CACHE_SET_ASSOC   8
#define CPU_L1_LINE_OFFSET_WORD  (1 << 12)

#define l1_index(_line, _set)\
    (((CPU_L1_LINE_OFFSET_WORD) * (_line)) / 8 + \
    (((CPU_L1_CACHE_SET_ASSOC) * (_set))))

#define mfence() asm volatile("mfence;");
#define lfence() asm volatile("lfence;");
#define rdtscp() asm volatile("rdtscp;");
#define clflush(_addr) asm volatile("clflush (%0)" : : "r" (_addr));
#define maccess(_addr) asm volatile("movq (%0), %%rax\n" : : "r" (_addr) );



#define mrdtscp(_mrdtscp_code, _t) ({\
    volatile register uint32_t _delta;\
    rdtscp();\
    asm volatile("mov %rax, %r10;");\
    _mrdtscp_code;\
    rdtscp();\
    asm volatile("sub %%r10, %%rax;" : "=a" (_delta));\
    *(_t) = _delta;\
});

#define deref_8(_ptr) ({\
    asm volatile (\
    ".rept 8;"\
        "mov (%0), %0;"\
    ".endr;"\
    : : "r" (_ptr));\
}) 

#define l1_prime(_base, _set)({\
    volatile register uint64_t * _ptr;\
    _ptr = _base + l1_index(0, _set);\
    deref_8(_ptr);\
});

#define iterate_asm(_iter, _iterate_code, _id)({\
    asm volatile("mov %0, %%r11;" :  : "n" (_iter));\
    asm volatile("LOOP%0:" : : "n" (_id));\
    _iterate_code;\
    asm volatile("dec %r11;");\
    asm volatile("jnz LOOP%0;" : : "n" (_id));\
});

#define l1_prime_next l1_prime(__MEM_BASE__, __CURRENT_SET__); __CURRENT_SET__++

#define l1_probe(_base, _set, _t)({\
    volatile register uint64_t * _ptr;\
    _ptr =_base + l1_index(7, _set) + 1;\
    mfence();\
    mrdtscp(deref_8(_ptr), _t);\
});

#define l1_probe_next l1_probe(__MEM_BASE__, __CURRENT_SET__, __TIME_BUFFER__); \
    __CURRENT_SET__++; __TIME_BUFFER__++



#define flush_reload(_target, _t)({\
    volatile register uint64_t * _addr = (uint64_t *)_target;\
    mfence();\
    mrdtscp(maccess(_addr), _t);\
    clflush(_addr);\
});

#define flush_flush(_target, _t)({\
    volatile register uint64_t * _addr = (uint64_t *)_target;\
    mfence();\
    mrdtscp(clflush(_addr), _t);\
});


#define warmup() for(uint32_t i = 0; i < 10000000; i++);