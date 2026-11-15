#include "global.h"
int AnimatedParticle_PriorityFromFlags(int flags)
{
    int priority;
    int result;

    flags &= 0xf000;
    flags >>= 12;
    priority = flags - 1;
    result = 2;
    if ((u32)priority <= 3)
        result = priority;
    return result;
}
