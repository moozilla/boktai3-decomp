#include "global.h"
void DirectionBurst_Emit(u8 *, s32, s32, s32);
void sub_0819EB84(u8 *p, s32 angle, s32 count, s32 step)
{
    s32 i;
    angle = (angle - ((count - 1) * step >> 1)) & 0xff;
    for (i = 0; i < count; i++) {
        DirectionBurst_Emit(p, (i * step + angle) & 0xff, 12, i + 128);
    }
}
