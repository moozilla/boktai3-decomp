#include "global.h"
#include "player.h"
#ifdef NONMATCHING
// fuzzy: 84.6%
// foundry draft (worker best 84.6%); what differs is mostly register allocation and instruction order
#define PlayerAnimation_SelectDirectionalKeyframes Player_SetAnimation
#define Animation_SetPlaybackRate                  Anim_SetFrameDurationScale
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
void Anim_SetFrameDurationScale(void *, u32);
s32 Player_SetAnimation(void *p, u32 index, u32 rate, u32 mode)
{
    struct Player *actor;
    struct PlayerAnimation *anim;
    void *data;
    u32 flags = 4;
    actor = p;
    if (actor->animationIndex == index)
        flags = 5;
    actor->animationIndex = index;
    anim = &actor->animation;
    data = actor->animationData;
    Anim_SetKeyframes(anim, data, (u16)(index + (u32)actor->heading), (u16)mode, flags);
    if (actor->side)
        anim->flags |= 4;
    else
        anim->flags &= ~4;
    Anim_SetFrameDurationScale(anim, (u16)rate);
    return Anim_Step(anim, data);
}
#else
INCLUDE_ASM("asm/nonmatching", Player_SetAnimation);
#endif
