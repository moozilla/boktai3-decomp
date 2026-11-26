#include "global.h"
void DirectionBurst_Emit(u8 *, s32, s32, s32);
void sub_0819EBD0(u8 *p, s32 angle, s32 count, s32 step, s32 kind)
{
    s32 i;
    for (i = 0; i < count; i++) {
        DirectionBurst_Emit(p, (angle + i * step) & 0xff, kind, 128);
    }
}
