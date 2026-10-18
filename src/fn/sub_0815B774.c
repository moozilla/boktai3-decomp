#include "global.h"
#include "player.h"
#include "engine.h"
void Player_EnterKindTwo(struct Player *);
s32 ActorVariant_GetTableValue(u32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void sub_08151078(struct Player *);
void PlayerCue_BeginOddDirection(struct Player *p, s32 direction)
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
    Player_EnterKindTwo(p);
    Anim_SetKeyframes(&p->animation, p->animationData,
                      (u16)(p->heading + ActorVariant_GetTableValue(p->actionMode, 0x2e)), 1, 2);
    Anim_Step(&p->animation, p->animationData);
    if (p->side)
        p->animation.flags |= 4;
    else
        p->animation.flags &= ~4;
    ActorSequence_Begin(p, 0x1d, 2);
    p->triggerAnimationCallback = sub_08151078;
}
