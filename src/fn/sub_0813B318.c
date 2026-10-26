#include "global.h"
#include "game_state.h"

struct P0813B318 {
    u8 f0[0x444];
    u32 flags;
    u8 f448[0x2A];
    u8 idx;
};
struct G0813B318 {
    u8 f0[0x628];
    s32 arr[1];
};
static inline u32 tst(u32 *a, u32 m)
{
    return *a & m;
}
s32 SunClock_GetPhase(void);

s32 Player_IsActionBlocked(struct P0813B318 *p)
{
    s32 v;
    if (*(s32 *)((u8 *)gGameState + p->idx * 4 + 0x628) > 0 ||
        (p != 0 && tst(&p->flags, 0x4000) != 0) ||
        ((v = SunClock_GetPhase()), (u32)(v - 4) <= 1 || v == 0))
        return 1;
    return 0;
}
