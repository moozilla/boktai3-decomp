#include "global.h"
#include "player.h"
// MATCHING_CONCESSION: register-pinning
#define Animation_SetPlaybackRate Anim_SetFrameDurationScale
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
void Anim_SetFrameDurationScale(void *, u32);
s32 PlayerAnimation_SelectSecondaryKeyframes(struct Player *p, s32 index, s32 rate)
{
    struct PlayerAnimation *anim;
    register void *data asm("r5");
    u32 flags = 0;
    if (p->animationIndex == index)
        flags = 1;
    p->animationIndex = index;
    anim = &p->secondaryAnimation;
    data = p->animationData;
    {
        s32 selected = index;
        selected = selected + p->heading;
        Anim_SetKeyframes(anim, data, (u16)selected, 1, flags);
    }
    if (p->side)
        anim->flags |= 4;
    else
        anim->flags &= ~4;
    Anim_SetFrameDurationScale(anim, (u16)rate);
    return Anim_Step(anim, data);
}
