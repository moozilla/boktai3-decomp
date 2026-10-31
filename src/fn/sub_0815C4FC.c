#include "player.h"

void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_UpdateFacingSide(struct Player *);
void PlayerCue_UpdateSpiralMotion(struct Player *);
void PlayerCue_UpdateSpiralRecovery(struct Player *);
void Player_UpdateStateMotionCue(struct Player *);

u32 PlayerCue_StateMotion(struct Player *p, u32 c)
{
    p->cueParameter = c;
    if (p->actionMode <= 1 || p->actionMode == 6)
        return 0;
    if (p->actionMode == 3) {
        Player_EnterKindTwo(p);
        ActorSequence_Begin(p, 0xD, 0);
        p->triggerAnimationCallback = PlayerCue_UpdateSpiralMotion;
    } else if (p->actionMode == 4) {
        Player_EnterKindTwo(p);
        ActorSequence_Begin(p, 0xE, 0);
        p->triggerAnimationCallback = PlayerCue_UpdateSpiralRecovery;
    } else if (p->actionMode == 5) {
        p->facing = 4;
        Player_UpdateFacingSide(p);
        Player_EnterKindTwo(p);
        ActorSequence_Begin(p, 0xF, 0);
        p->triggerAnimationCallback = Player_UpdateStateMotionCue;
    } else {
        return 0;
    }
    return 1;
}
