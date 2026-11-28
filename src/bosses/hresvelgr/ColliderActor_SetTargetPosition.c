#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"
#include "scene/motion/scene_facing.h"
u16 Sqrt(u32);
static inline void SetTravelTime(struct HresvelgrTravelActor *actor, u32 time)
{
    actor->travel = time;
    if ((u16)time <= 1)
        actor->travel = 2;
}
s32 ColliderActor_SetTargetPosition(struct HresvelgrTravelActor *actor, struct SceneVec *target)
{
    struct SceneVec delta;
    struct SceneVec *out;
    struct SceneVec *position;
    s32 distance;
    actor->target = *target;
    actor->facing = (actor->side << 6) | 0x80u;
    actor->facing4 = SceneFacing_FourWay(actor->facing);
    actor->facing8 = SceneFacing_EightWay(actor->facing);
    out = &delta;
    position = &actor->position;
    delta.x = target->x - position->x;
    out->y = target->y - position->y;
    out->z = target->z - position->z;
    distance = Sqrt(delta.x * delta.x + out->z * out->z);
    SetTravelTime(actor, distance * 628 / 25600);
    /* All three direct ROM callers discard the incidental return register. */
}
