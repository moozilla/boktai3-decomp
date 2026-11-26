#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"
#include "scene/motion/scene_facing.h"

void Hresvelgr_SetAnimation(struct HresvelgrTravelActor *, void *, void *, u32, u32, u32);

static inline void SetHeading(struct HresvelgrTravelActor *p, s32 angle)
{
    p->facing = angle;
    p->facing4 = SceneFacing_FourWay(p->facing);
    p->facing8 = SceneFacing_EightWay(p->facing);
}

void Hresvelgr_BeginRestAnimation(struct HresvelgrTravelActor *actor)
{
    if (actor->changed) {
        actor->changed = 0;
        SetHeading(actor, actor->savedHeading);
        Hresvelgr_SetAnimation(actor, &actor->animation, &actor->animationData, 26, 2, 4);
    }
    actor->motionX = 0;
    actor->motionY = 0;
    actor->motionZ = 0;
    actor->motionW = 0;
    Hresvelgr_SetFlags(actor, 12);
}
