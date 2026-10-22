// MATCHING_CONCESSION: register-pinning
#include "global.h"
u32 SceneActor_GetDirectionFlags(s32 direction)
{
    register s32 result asm("r0") = direction;
    register s32 quadrant asm("r1") = (direction + 16) & 255;
    asm("" : "+r"(quadrant));
    quadrant >>= 6;
    asm("" : "+r"(quadrant), "+r"(result));
    switch (quadrant) {
        case 0:
        case 2:
            result = 0;
            break;
        case 1:
        case 3:
            result = 1;
            break;
    }
    asm("" : "+r"(result));
    return result;
}
