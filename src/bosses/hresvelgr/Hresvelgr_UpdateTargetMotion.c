#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"
#include "scene/motion/scene_facing.h"
void Hresvelgr_SetAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void Hresvelgr_SetRandomTravelTarget(struct HresvelgrTravelActor *);
void Hresvelgr_SteerTowardTarget(struct HresvelgrTravelActor *);
s32 SceneMover_IsNearTarget(void *, s32);
void Hresvelgr_UpdateTargetMotion(struct HresvelgrTravelActor *p)
{
    Hresvelgr_SetFlags(p, 0x400040);
    if (p->changed) {
        p->changed = 0;
        ZeroFill8(&p->delta);
        p->facing = p->targetHeading;
        p->facing4 = SceneFacing_FourWay(p->facing);
        p->facing8 = SceneFacing_EightWay(p->facing);
        Hresvelgr_SetAnimation(p, &p->animation, &p->animationData, 0, 1, 0);
        Hresvelgr_SetRandomTravelTarget(p);
    }
    if ((u8)Callback_InvokeActor(p, p->callback))
        return;
    if (p->pulse)
        Sound_PlaySong(0x3d9);
    if (!SceneMover_IsNearTarget(p, 4)) {
        Hresvelgr_SteerTowardTarget(p);
        if (++p->phaseFrame > 0x40) {
            p->phaseFrame = 1;
            Hresvelgr_SetRandomTravelTarget(p);
        }
    } else
        Hresvelgr_SetRandomTravelTarget(p);
}
