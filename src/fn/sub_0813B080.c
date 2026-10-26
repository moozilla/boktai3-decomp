#include "global.h"
#include "player.h"
#include "game_state.h"
s32 AreaEffect_TestPlayerCoverage(u8 *a);
static inline u32 masked(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline void set(u32 *p, u32 mask)
{
    *p |= mask;
}
static inline u32 counter(u32 slot, struct GameState *state)
{
    return (slot << 2) + (u32)state;
}
void Player_ResetStatus(struct Player *p)
{
    p->statusFlags = 1;
    if (*(s32 *)(counter(p->timedEventSlot, gGameState) + 0x628) < 0)
        *(s32 *)(counter(p->timedEventSlot, gGameState) + 0x628) = 0;
    if (*(s32 *)(counter(p->timedEventSlot, gGameState) + 0x628) == 0 &&
        AreaEffect_TestPlayerCoverage((u8 *)p))
        set(&p->statusFlags, 0x10);
    if (p && masked(&p->statBonusFlags, 0x2000))
        set(&p->statusFlags, 0x10000);
    p->resetValue6F8 = 0;
    p->resetValue6FA = 0;
    p->resetValue6FC = 0;
    p->resetValue41A = 0;
    p->resetState740 = 1;
    p->resetFlags &= ~2;
}
