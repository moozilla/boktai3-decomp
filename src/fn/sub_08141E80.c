#define Player_UpdateMovement           sub_08141E80
#define Player_GetSmoothedDirection     sub_08141BAC
#define Player_GetMovementSpeed         sub_08141AE4
#define Player_ComputeInverseSpeedScale Player_GetMovementRateScale
#define Player_SetDirectionalVelocity   sub_0813AF18
#define Player_UpdateFacingSide         sub_0813ADD0
#define Player_SelectMovementAnimation  sub_081419D4
#define Player_SetAnimation             sub_0813AE18
#define Player_AddStatA Player_IncreaseLifeTowardLimit
#define Player_AddStatB Player_AddEnergy
#define Player_SubtractStatB Player_SubtractEnergy
#include "global.h"
#include "player.h"
s32 sub_08141BAC(struct Player *);
s32 sub_08141AE4(struct Player *);
s32 Player_GetMovementRateScale(s32);
void sub_0813AF18(struct Player *, s32);
void sub_08141C40(struct Player *, s32);
void sub_0813ADD0(struct Player *);
s32 sub_081419D4(u32, s32);
s32 sub_0813AE18(struct Player *, s32, s32, s32);
void Player_IncreaseLifeTowardLimit(struct Player *, u32);
void Player_AddEnergy(struct Player *, s32);
void Player_SubtractEnergy(struct Player *, s32);
static inline u32 MaskFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline u32 HasFlag0(struct Player *p)
{
    if (!p)
        return 0;
    return MaskFlags(&p->actionFlags, 0x100);
}
static inline u32 HasFlag1(struct Player *p)
{
    if (!p)
        return 0;
    return MaskFlags(&p->actionFlags, 0x200);
}
static inline u32 HasFlag2(struct Player *p)
{
    if (!p)
        return 0;
    return MaskFlags(&p->actionFlags, 0x400);
}
static inline u8 IsModeFive(struct Player *p)
{
    u32 active = p->active;
    u8 result = 0;
    if (active && p->sceneMode == 5)
        result = 1;
    return result;
}
void sub_08141E80(struct Player *p)
{
    p->statusFlags &= ~1;
    p->facing = sub_08141BAC(p);
    if (!p->movementVelocityLocked) {
        s32 speed = sub_08141AE4(p);
        Player_GetMovementRateScale(speed);
        if (p && IsModeFive(p))
            speed = (p->movementSpeedScale * speed) >> 6;
        sub_0813AF18(p, speed);
    }
    sub_08141C40(p, 0);
    sub_0813ADD0(p);
    sub_0813AE18(p, sub_081419D4(p->actionMode, 1), 64, 1);
    if (++p->movementStatTimer > 60) {
        p->movementStatTimer = 0;
        if (HasFlag0(p))
            Player_IncreaseLifeTowardLimit(p, 1);
        if (HasFlag1(p))
            Player_AddEnergy(p, 1);
    }
    if (p->movementStatTimer == 20 || p->movementStatTimer == 40 || p->movementStatTimer == 60) {
        if (HasFlag2(p))
            Player_SubtractEnergy(p, 1);
    }
}
