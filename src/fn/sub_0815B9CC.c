#include "player.h"

void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_UpdateFacingSide(struct Player *);
void PlayerCue_UpdateMotionFade(struct Player *);
void PlayerCue_UpdateSideFade(struct Player *);
void PlayerCue_UpdateAlternateFade(struct Player *);
void PlayerCue_UpdateScaleTransition(struct Player *);

void PlayerCue_Motion7Variant3(struct Player *p, s32 dir, u32 c)
{
    if (dir >= 0)
        p->facing = dir;
    p->cueParameter = c;
    Player_EnterKindTwo(p);
    if (p->actionMode == 3) {
        p->facing |= 1;
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        ActorSequence_Begin(p, 7, 3);
        p->triggerAnimationCallback = PlayerCue_UpdateMotionFade;
    } else {
        Player_UpdateFacingSide(p);
        if (p->actionMode == 4) {
            ActorSequence_Begin(p, 7, 3);
            p->triggerAnimationCallback = PlayerCue_UpdateSideFade;
        } else if (p->actionMode == 5) {
            ActorSequence_Begin(p, 7, 3);
            p->triggerAnimationCallback = PlayerCue_UpdateAlternateFade;
        } else {
            ActorSequence_Begin(p, 7, 4);
            p->triggerAnimationCallback = PlayerCue_UpdateScaleTransition;
        }
    }
}
