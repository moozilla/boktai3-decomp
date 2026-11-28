#include "global.h"
#include "player/player.h"

s32 Player_GetLinkedSmoothedDirection(struct Player *);
s32 Player_ComputeLinkedActionDelay(struct Player *);
s32 Player_GetMovementAnimationRate(s32);
void LinkPlayer_SetScaledDirectionalVelocity(struct Player *, s32);
void Player_PlayActionCueSound(struct Player *, u32);
void Player_UpdateActionFacingSide(struct Player *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(struct Player *, s32, s32);

static inline s32 IsScaled(struct Player *p)
{
    u32 active = p->active;
    s32 scaled = 0;

    if (active != 0) {
        if (p->sceneMode == 5)
            scaled = 1;
    }
    return scaled;
}

void sub_0823C6BC(struct Player *p)
{
    s32 delay;

    p->tetherMode = 1;
    p->statusFlags &= ~1;
    p->facing = Player_GetLinkedSmoothedDirection(p);
    if (p->movementVelocityLocked == 0) {
        delay = Player_ComputeLinkedActionDelay(p);
        Player_GetMovementAnimationRate(delay);
        if (p != NULL && IsScaled(p))
            delay = (p->animation.scaleX * delay) >> 6;
        LinkPlayer_SetScaledDirectionalVelocity(p, delay);
    }
    Player_PlayActionCueSound(p, 0);
    Player_UpdateActionFacingSide(p);
    Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 1), 0x40);
}
