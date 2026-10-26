#include "global.h"
#include "game_state.h"

void PlayerAction_RefreshFlagState(u32, void *, s32);

s32 Player_GetCandidateAttribute(s32 i, s32 d)
{
    u8 buf[16];
    s32 r;
    u8 *q;
    u8 *g = gGameState;
    i <<= 1;
    q = (u8 *)i;
    q += (s32)g;
    r = *(s16 *)(q + 0x18) + *(s16 *)(q + 0x20);
    PlayerAction_RefreshFlagState(0, buf, d);
    {
        u8 *e = buf + 6;
        e += i;
        r += *(s16 *)e;
    }
    return r;
}
