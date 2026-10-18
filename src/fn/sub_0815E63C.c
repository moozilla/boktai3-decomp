#define PlayerContact_ApplyResponse   sub_0815E1F0
#define gGameState                    gUnk_02000710
#define PlayerContact_ApplyStatReward sub_0815E63C
#define PlayerStats_AddHundredths     sub_0815977C
#define PlayerCue_InitCollider        sub_0815DC48
#include "global.h"
#include "player.h"
#include "contact_actor.h"
#include "game_state.h"
s32 Div(s32, s32);
void sub_0815E1F0(struct Player *, struct ContactCollider *, struct ContactCollider *);
void sub_0815977C(struct Player *, s32);
void sub_0815DC48(struct Player *);
static inline u32 masked(u32 *p, u32 mask)
{
    return *p & mask;
}
void sub_0815E63C(struct ContactCollider *a, struct ContactCollider *b, struct Player *p)
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
    sub_0815E1F0(p, a, b);
    if (!(a->statRewardFlags & 0x400000)) {
        value = Div(b->cost << 8, p->maximumStatA) * 50;
        if (value >= 0)
            gain = value >> 8;
        else
            gain = -(-value >> 8);
        sub_0815977C(p, gain);
        if (p->actionMode == 1)
            gUnk_02000710->unk600 += 12;
    }
    if (p && masked(&p->statBonusFlags, 0x4000000))
        sub_0815DC48(p);
}
