#include "player_cue.h"

void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_UpdateTargetDash(struct Player *);

u32 PlayerCue_Motion38To(struct Player *p, const struct SceneVec *pos, u32 c)
{
    u32 r;

    p->cueParameter = c;
    if (p->actionMode == 2) {
        Player_EnterKindTwo(p);
        ActorSequence_Begin(p, 0x26, 0);
        p->triggerAnimationCallback = PlayerCue_UpdateTargetDash;
        PlayerCue_Motion(p)->destination.x = pos->x;
        PlayerCue_Motion(p)->destination.y = pos->y;
        PlayerCue_Motion(p)->destination.z = pos->z;
        r = 1;
    } else {
        r = 0;
    }
    return r;
}
