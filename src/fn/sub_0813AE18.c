#include "global.h"
#include "player.h"
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
    {
        u32 sum = index;
        sum += actor->heading;
        Anim_SetKeyframes(&actor->animation, data, (u16)sum, (u16)mode, flags);
    }
    if (actor->side)
        anim->flags |= 4;
    else
        (&actor->animation)->flags &= ~4;
    Anim_SetFrameDurationScale(&actor->animation, (u16)rate);
    return Anim_Step(&actor->animation, actor->animationData);
}
