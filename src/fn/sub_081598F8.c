#define PlayerMovement_ClearModifiers Player_EmitQueuedEffects
#include "global.h"
#include "player.h"
#include "ram.h"
void Player_EmitQueuedEffects(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
static inline void SetFlags(u32 *flags, u32 mask)
{
    *flags |= mask;
}
void PlayerAction_BeginMode24(struct Player *p)
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
    gPlayerPendingActor = 0;
    gPlayerNotificationPending = 0;
    ActorSequence_Begin(p, 0x18, 0);
}
