#include "global.h"
#include "player.h"
void Player_UpdateFacingSide(struct Player *);
void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void sub_08151A14(struct Player *);
void PlayerCue_BeginModeSeven(struct Player *p, s32 mode)
{
    if (mode == 0) {
        p->targetFacing = 1;
        Player_UpdateFacingSide(p);
        p->heading--;
        p->tetherPhase = 0;
    } else if (mode == 1) {
        p->targetFacing = 5;
        Player_UpdateFacingSide(p);
        p->heading--;
        p->tetherPhase = 0;
    } else if (mode == 2) {
        p->cuePhase = 0;
        p->tetherPhase = 5;
    } else if (mode == 3) {
        p->targetFacing = mode;
        Player_UpdateFacingSide(p);
        p->heading--;
        p->tetherPhase = 0;
    } else return;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 7, p->tetherPhase);
    p->triggerAnimationCallback = sub_08151A14;
}
