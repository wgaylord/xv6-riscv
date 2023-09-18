#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "memlayout.h"

struct fdt_header {
    uint32 magic;
    uint32 totalsize;
    uint32 off_dt_struct;
    uint32 off_dt_strings;
    uint32 off_mem_rsvmap;
    uint32 version;
    uint32 last_comp_version;
    uint32 boot_cpuid_phys;
    uint32 size_dt_strings;
    uint32 size_dt_struct;
};

struct fdt_reserve_entry {
    uint64 address;
    uint64 size;
};


void freerange(void *pa_start, void *pa_end);


static inline uint32 roundup_32(uint32 x)
{
    if(x > (x&0xFFFFFFFC)){
        return (x&0xFFFFFFFC) + 4;
    }
    return x;

}

