#define PlayerAction_BeginMode24      sub_081598F8
#define PlayerAction_SetModePhase     sub_0813AFEC
#define PlayerMovement_ClearModifiers Player_EmitQueuedEffects
#include "global.h"
#include "player.h"
extern void *gUnk_0200054C;
extern u16 gUnk_02000594;
void Player_EmitQueuedEffects(struct Player *);
void sub_0813AFEC(struct Player *, s32, s32);
static inline void SetFlags(u32 *flags, u32 mask)
{
    *flags |= mask;
}
void sub_081598F8(struct Player *p)
{
    SetFlags(&p->statusFlags, 0x1000);
    p->actionLatch464 = 0;
    p->heading = 0;
    Player_EmitQueuedEffects(p);
    p->collider.flags |= 4;
    p->directionFrame = 0;
    p->positionResetValue = 0;
    p->positionResetTimer = 0;
    p->positionResetCounter = 0;
    p->positionResetState = 0;
    p->particleEmissionCount = 0;
    gUnk_0200054C = 0;
    gUnk_02000594 = 0;
    sub_0813AFEC(p, 0x18, 0);
}
