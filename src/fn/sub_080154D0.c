#include "global.h"
// MATCHING_CONCESSION: register-pinning
int AnimatedParticle_PriorityFromFlags(int flags)
{
    register int p asm("r0") = flags & 0xf000;
    int result;
    p = (p >> 12) - 1;
    result = 2;
    if ((u32)p <= 3)
        result = p;
    return result;
}
