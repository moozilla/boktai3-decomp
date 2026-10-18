#define Direction_IsMiddleQuadrants sub_081218A0
#include "global.h"
u32 sub_081218A0(s32 direction)
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
