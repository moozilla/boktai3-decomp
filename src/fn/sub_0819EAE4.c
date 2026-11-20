#include "global.h"
#define Callback_InvokeActor _call_via_r1
#include "hresvelgr_travel.h"

#include "scene_facing.h"
void Hresvelgr_SetAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void ActorAction_UpdateDirectionalAnimation(struct HresvelgrTravelActor *p)
{
    p->facing = p->targetHeading;
    p->facing4 = SceneFacing_FourWay(p->facing);
    p->facing8 = SceneFacing_EightWay(p->facing);
    {
        u32 mask = 0x200040;
        p->flags |= mask;
    }
    if (p->changed) {
        p->changed = 0;
        Hresvelgr_SetAnimation(p, &p->animation, &p->animationData, 0x10, 1, 4);
        Sound_PlaySong(0x4D0);
    }
    if (p->pulse && p->animationCompletions > 5)
        p->finished = 1;
    Callback_InvokeActor(p, p->callback);
}
