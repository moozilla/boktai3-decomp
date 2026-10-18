#define Player_ComputeInverseSpeedScale Player_GetMovementRateScale
#define Player_AddStatA Player_IncreaseLifeTowardLimit
#define Player_AddStatB Player_AddEnergy
#define Player_SubtractStatB Player_SubtractEnergy
#include "global.h"
#include "player.h"
s32 Player_GetSmoothedDirection(struct Player *);
s32 Player_GetMovementSpeed(struct Player *);
s32 Player_GetMovementRateScale(s32);
void Player_SetDirectionalVelocity(struct Player *, s32);
void sub_08141C40(struct Player *, s32);
void Player_UpdateFacingSide(struct Player *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
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
void Player_UpdateMovement(struct Player *p)
{
    p->statusFlags &= ~1;
    p->facing = Player_GetSmoothedDirection(p);
    if (!p->movementVelocityLocked) {
        s32 speed = Player_GetMovementSpeed(p);
        Player_GetMovementRateScale(speed);
        if (p && IsModeFive(p))
            speed = (p->movementSpeedScale * speed) >> 6;
        Player_SetDirectionalVelocity(p, speed);
    }
    sub_08141C40(p, 0);
    Player_UpdateFacingSide(p);
    Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 1), 64, 1);
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
