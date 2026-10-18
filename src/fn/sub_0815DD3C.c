#define gGameState gUnk_02000710
#include "global.h"
#include "game_state.h"
s32 Div(s32, s32);
static inline s32 ScaleSigned(s32 x)
{
    if (x >= 0) x >>= 6;
    else x = -(-x >> 6);
    return x;
}
s32 Player_ScaleLevelValue(u32 flags, s32 scale)
{
    s32 value = gUnk_02000710->level + 50;
    s32 adjustment;
    if (flags & 2)
        adjustment = (10 - gUnk_02000710->unk876) * (value << 1);
    else {
        s32 amount = gUnk_02000710->unk876;
        adjustment = (value << 1) * amount;
    }
    value += Div(adjustment, 100);
    value = ScaleSigned(value * scale);
    return value;
}
