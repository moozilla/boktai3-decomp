#define PlayerCue_BeginModeSeven sub_0815BE5C
#include "global.h"
#include "player.h"
void sub_0813ADD0(struct Player *);
void sub_0815AD78(struct Player *);
void sub_0813AFEC(struct Player *, s32, s32);
void sub_08151A14(struct Player *);
void sub_0815BE5C(struct Player *p, s32 mode)
{
    if (mode == 0) {
        p->targetFacing = 1;
        sub_0813ADD0(p);
        p->heading--;
        p->tetherPhase = 0;
    } else if (mode == 1) {
        p->targetFacing = 5;
        sub_0813ADD0(p);
        p->heading--;
        p->tetherPhase = 0;
    } else if (mode == 2) {
        p->cuePhase = 0;
        p->tetherPhase = 5;
    } else if (mode == 3) {
        p->targetFacing = mode;
        sub_0813ADD0(p);
        p->heading--;
        p->tetherPhase = 0;
    } else return;
    sub_0815AD78(p);
    sub_0813AFEC(p, 7, p->tetherPhase);
    p->triggerAnimationCallback = sub_08151A14;
}
