#include "global.h"
#include "player.h"
#include "contact_actor.h"
#include "game_state.h"
s32 Div(s32, s32);
void PlayerContact_ApplyResponse(struct Player *, struct ContactCollider *,
                                 struct ContactCollider *);
void PlayerStats_AddHundredths(struct Player *, s32);
void PlayerCue_InitCollider(struct Player *);
static inline u32 masked(u32 *p, u32 mask)
{
    return *p & mask;
}
void PlayerContact_ApplyStatReward(struct ContactCollider *a, struct ContactCollider *b,
                                   struct Player *p)
{
    s32 value, gain;
    u32 flags;
    u32 mask;
    if (p->kind != 1)
        return;
    mask = 0x200;
    flags = a->flags;
    if (flags & mask) {
        if (!(b->responseFlags & 1))
            return;
        b->responseFlags &= ~1;
    }
    if (flags & 0x400) {
        if (b->responseFlags & 2)
            return;
        b->responseFlags |= 2;
    }
    PlayerContact_ApplyResponse(p, a, b);
    if (!(a->statRewardFlags & 0x400000)) {
        value = Div(b->cost << 8, p->maximumStatA) * 50;
        if (value >= 0)
            gain = value >> 8;
        else
            gain = -(-value >> 8);
        PlayerStats_AddHundredths(p, gain);
        if (p->actionMode == 1)
            gGameState->unk600 += 12;
    }
    if (p && masked(&p->statBonusFlags, 0x4000000))
        PlayerCue_InitCollider(p);
}
