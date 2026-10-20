#include "global.h"
#include "player.h"
#include "engine.h"
void Anim_SetFrameDurationScale(void *, u32);

s32 Player_SetAnimationIndex(struct Player *p, s32 index, u32 scale)
{
    u32 same = 0;
    if (p->animationIndex == index)
        same = 1;
    p->animationIndex = index;
    /* index - -heading: operand order that agbcc keeps as r1 + r2 */
    Anim_SetKeyframes(&p->animation, p->animationData, (u16)(index - -p->heading), 1, same);
    if (p->side)
        p->animation.flags |= 4;
    else
        p->animation.flags &= ~4;
    Anim_SetFrameDurationScale(&p->animation, (u16)scale);
    return Anim_Step(&p->animation, p->animationData);
}
