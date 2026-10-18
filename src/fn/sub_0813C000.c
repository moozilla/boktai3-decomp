#define DirectionalEffect_GetAnimationParams sub_0813C000
#include "global.h"
void sub_0813C000(u32 mode, u32 *variant, u32 *kind)
{
    switch ((s32)(mode & 3)) {
    case 0: *variant = 0; break;
    case 1: *variant = 1; break;
    case 2: *variant = 2; break;
    default: *variant = 1; break;
    }
    if (mode <= 2) *kind = 0;
    else if (mode - 3 <= 1) *kind = 2;
    else if (mode == 5) *kind = 3;
    else *kind = 1;
}
