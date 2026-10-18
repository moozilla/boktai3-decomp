#include "global.h"
u32 Direction_IsMiddleQuadrants(s32 direction)
{
    direction = ((direction + 32) >> 6) & 3;
    switch (direction) {
        case 1:
        case 2:
            return 1;
        default:
            return 0;
    }
}
