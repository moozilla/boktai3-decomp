#include "global.h"
/* Tentative: caller derives quadrant from x/y, then selects a sprite animation. */
s32 DirectionMarker_GetAnimation(s32 base, s32 group)
{
    if (group <= 1)
        base += 0x27;
    else
        base += 0x23;
    return base;
}
