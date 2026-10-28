#include "global.h"
#include "engine.h"
#include "ram.h"

s32 Mod(s32, s32);

void ScenePool_SetLateralControlSchedule(u8 *p, s32 a, s32 lo, s32 hi)
{
    s32 n;
    s32 v;
    *(s32 *)(p + 0x174) = a;
    n = hi - lo;
    v = lo;
    if (n != 0) {
        gRandomIndex = (gRandomIndex + 1) & 0x3ff;
        v += Mod(*(const u16 *)((gRandomIndex << 1) + (u32)gRandomTable), n);
    }
    *(s32 *)(p + 0x178) = v;
}
