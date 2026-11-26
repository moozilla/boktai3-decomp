#define PlayerMovement_ClearModifiers Player_EmitQueuedEffects
#include "engine/shared/global.h"
#include "engine/shared/player.h"
#include "engine/shared/ram.h"
#include "engine/shared/player_slot_status.h"
void Player_EmitQueuedEffects(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerAction_BeginMode24(struct Player *p)
{
    Player_SetStatusBits(&p->statusFlags, 0x1000);
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
