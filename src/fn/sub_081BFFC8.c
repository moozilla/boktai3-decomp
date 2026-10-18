#include "global.h"
#define DirectionMarker_GetAnimation sub_081BFFC8
/* Tentative: caller derives quadrant from x/y, then selects a sprite animation. */
s32 sub_081BFFC8(s32 base, s32 group)
{
    if (group <= 1) base += 0x27;
    else base += 0x23;
    return base;
}
