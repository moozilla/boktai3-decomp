#include "global.h"
void DirectionalEffect_DecodeAnimationMode(u32 mode, s32 *variant, s32 *group)
{
    s32 v = mode & 3;
    switch (v) {
        case 0:
            v = 0;
            break;
        case 1:
            v = 1;
            break;
        case 2:
            v = 2;
            break;
        default:
            v = 1;
            break;
    }
    *variant = v;
    if (mode <= 2)
        *group = 0;
    else if (mode - 3 <= 1)
        *group = 2;
    else if (mode == 5)
        *group = 3;
    else
        *group = 1;
}
