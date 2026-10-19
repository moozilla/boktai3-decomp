#define Player_ApplyPeriodicStatChanges sub_0813E53C
#define Player_AddStatA Player_IncreaseLifeTowardLimit
#define Player_SubtractStatA Player_SubtractLifeNonlethal
#define Player_AddStatB Player_AddEnergy
#define Player_SubtractStatB Player_SubtractEnergy
#include "global.h"
#include "player.h"
s32 Div(s32, s32);
void Player_IncreaseLifeTowardLimit(struct Player *, s32);
void Player_SubtractLifeNonlethal(struct Player *, s32);
void Player_AddEnergy(struct Player *, s32);
void Player_SubtractEnergy(struct Player *, s32);
static inline u32 TestFlags(u32 *flags, u32 mask)
{
    u32 m = mask;
    return *flags & m;
}
static inline s32 HasFlag(struct Player *p, u32 mask)
{
    if (!p)
        return FALSE;
    if (TestFlags(&p->statBonusFlags, mask))
        return TRUE;
    return FALSE;
}
void sub_0813E53C(struct Player *p)
{
    s32 amount;
    if (p->kind == 1) {
        if (HasFlag(p, 4)) {
            amount = Div(p->maximumStatA, 100);
            if (amount <= 0)
                amount = 1;
            Player_IncreaseLifeTowardLimit(p, amount);
        }
        if (HasFlag(p, 8)) {
            amount = Div(p->maximumStatA, 100);
            if (amount <= 0)
                amount = 1;
            Player_SubtractLifeNonlethal(p, amount);
        }
        if (HasFlag(p, 16)) {
            amount = Div(p->maximumStatB, 100);
            if (amount <= 0)
                amount = 1;
            Player_AddEnergy(p, amount);
        }
        if (HasFlag(p, 32)) {
            amount = Div(p->maximumStatB, 100);
            if (amount <= 0)
                amount = 1;
            Player_SubtractEnergy(p, amount);
        }
    }
}
