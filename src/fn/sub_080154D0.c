// MATCHING_CONCESSION: register-pinning
#include "global.h"
int AnimatedParticle_PriorityFromFlags(int flags)
{
    int p_2;
    int p = flags & 0xf000;
    int result;
    DEBUG_NOP();
    p_2 = (p >> 12) - 1;
    result = 2;
    if (((u32)p_2) <= 3) {
        result = p_2;
    }
    return result;
}
