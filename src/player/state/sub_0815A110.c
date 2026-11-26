#include "global.h"

s32 Player_TryStartStun(u8 *, s32, s32);

s32 Player_StartForcedStun(u8 *a, s32 b)
{
    return Player_TryStartStun(a, b, 1);
}
