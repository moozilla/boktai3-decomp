#define PlayerCue_BeginOddDirection sub_0815B774
#include "global.h"
#include "player.h"
#include "engine.h"
void sub_0815AD78(struct Player *);
s32 sub_081419D4(u32, s32);
void sub_0813AFEC(struct Player *, s32, s32);
void sub_08151078(struct Player *);
void sub_0815B774(struct Player *p, s32 direction)
{
    if (direction >= 0)
        p->facing = direction;
    p->facing |= 1;
    if (p->facing > 4) {
        p->heading = (8 - p->facing) >> 1;
        p->side = 1;
    } else {
        p->heading = p->facing >> 1;
        p->side = 0;
    }
    sub_0815AD78(p);
    Anim_SetKeyframes(&p->animation, p->animationData,
        (u16)(p->heading + sub_081419D4(p->actionMode, 0x2e)), 1, 2);
    Anim_Step(&p->animation, p->animationData);
    if (p->side)
        p->animation.flags |= 4;
    else
        p->animation.flags &= ~4;
    sub_0813AFEC(p, 0x1d, 2);
    p->triggerAnimationCallback = sub_08151078;
}
