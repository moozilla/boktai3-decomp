#include "global.h"
#include "game_state.h"
s32 Div(s32, s32);
static inline s32 ScaleSigned(s32 x)
{
    if (x >= 0)
        x >>= 6;
    else
        x = -(-x >> 6);
    return x;
}
s32 Player_ScaleLevelValue(u32 flags, s32 scale)
{
    s32 value = gGameState->level + 50;
    s32 adjustment;
    if (flags & 2)
        adjustment = (10 - gGameState->unk876) * (value << 1);
    else {
        s32 amount = gGameState->unk876;
        adjustment = (value << 1) * amount;
    }
    value += Div(adjustment, 100);
    value = ScaleSigned(value * scale);
    return value;
}
