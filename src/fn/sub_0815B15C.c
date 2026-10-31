#include "player_cue.h"
#include "contact_position.h"

void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_UpdateMoveDestination(struct Player *);

void PlayerCue_MoveTo(struct Player *p, const struct ContactPosition *position, s32 facing,
                      s32 animated, u32 parameter)
{
    if (position == 0)
        PlayerCue_Coordinates(p)->destination = PlayerCue_Coordinates(p)->position;
    else
        PlayerCue_Coordinates(p)->destination = *position;
    p->cueParameter = parameter;
    p->animationBranchRequested = animated;
    p->targetFacing = facing;
    p->cuePhase = 0x96;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 6, 0);
    p->triggerAnimationCallback = PlayerCue_UpdateMoveDestination;
}
