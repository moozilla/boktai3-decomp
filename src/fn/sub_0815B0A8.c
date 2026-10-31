#include "player_cue.h"
#include "contact_position.h"

void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_UpdateWalkTarget(struct Player *);
void PlayerCue_UpdateSideWalkTarget(struct Player *);
void Player_AdvanceScriptDestination(struct Player *);

void PlayerCue_WalkTo(struct Player *p, const struct ContactPosition *pos, u32 c, s32 a)
{
    PlayerCue_Coordinates(p)->destination = *pos;
    p->cueParameter = c;
    p->targetFacing = a;
    if (p->actionMode == 3) {
        if ((s8)a >= 0)
            p->targetFacing = ((s8)a >> 1) * 2 + 1;
        p->triggerAnimationCallback = PlayerCue_UpdateWalkTarget;
    } else if (p->actionMode == 4) {
        p->triggerAnimationCallback = PlayerCue_UpdateSideWalkTarget;
    } else if (p->actionMode == 5) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
        p->triggerAnimationCallback = Player_AdvanceScriptDestination;
    } else {
        p->triggerAnimationCallback = Player_AdvanceScriptDestination;
    }
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 1, 0);
}
