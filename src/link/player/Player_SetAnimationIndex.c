#include "global.h"
#include "player/player.h"
#include "engine/engine.h"
void Anim_SetFrameDurationScale(void *, u32);

s32 Player_SetAnimationIndex(struct Player *p, s32 index, u32 scale)
{
    u32 same = p->animationIndex == index;

    p->animationIndex = index;
    Anim_SetKeyframes(&p->animation, p->animationData, (u16)(p->animationIndex + p->heading), 1,
                      same);
    if (p->side)
        p->animation.flags |= 4;
    else
        p->animation.flags &= ~4;
    Anim_SetFrameDurationScale(&p->animation, (u16)scale);
    return Anim_Step(&p->animation, p->animationData);
}
