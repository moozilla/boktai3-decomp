#include "global.h"
int AnimatedParticle_PriorityFromFlags(int flags)
{
    int p_2;
    union { int value; unsigned short half[2]; } control;
    int p;
    int result;
    control.value = flags;
    p = control.half[0] & 0xf000;
    p_2 = (p >> 12) - 1;
    result = 2;
    if (((u32)p_2) <= 3) {
        result = p_2;
    }
    return result;
}
