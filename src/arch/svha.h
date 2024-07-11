/* Copyright (C) 2024 by Federico Sauter <federicosauter@pm.me>. All rights reserved. */

#if !defined(_RADON_SVHA__H_)
#define _RADON_SVHA__H_

#include <sys/types.h>

/** A minimal hardware abstraction for the supervisor */
struct svha
{
    /** Perform a system call */
    uint32_t (*do_syscall)(unsigned, unsigned, unsigned);

    /** Halts the system (for kernel panics) */
    void (*halt)();

    uint32_t avail_mem_phy_addr;
    uint32_t avail_mem_len;
};

/** @return a pointer to the hardware abstraction for the current architecture
 */
const struct svha* sv_get_ha();

#endif /* _RADON_SVHA__H_ */
