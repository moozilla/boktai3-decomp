#include "actors/common_scene_actor_controller/scene_actors.h"
#include "engine/shared/scene_actor_charge_offset.h"
#include "engine/shared/scene_graphics_actor.h"

static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
static inline void decay(s16 *p)
{
    if (*p > 23)
        *p -= 24;
    else
        *p = 0;
}
void SceneActorGraphics_SelectResource(void *);
void SceneActors_UpdateKnockbackReaction(struct StateActor *base, int time)
{
    int tick = time;
    struct StateActor *p = base;
    u8 *position = (u8 *)p + 0x48;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        sequence = choose(((struct SceneGraphicsActor *)p)->graphics.animationFlags, 49, 11);
        SceneActors_StartDirectionalAnimation(
            p, &((struct SceneGraphicsActor *)p)->animation,
            (int)&((struct SceneGraphicsActor *)p)->animationResource, sequence, 2, 1, 4);
    }
    SceneGraphicsActor_ClearPendingDamage((struct SceneGraphicsActor *)p);
    SceneActorState_SetFlags(base, 12);
    if (((struct SceneGraphicsActor *)p)->graphics.scaleMin > 23)
        ((struct SceneGraphicsActor *)p)->graphics.scaleMin -= 24;
    else
        ((struct SceneGraphicsActor *)p)->graphics.scaleMin = 0;
    decay(&((struct SceneGraphicsActor *)p)->graphics.scaleMax);
    if (!base->actionReset(base)) {
        if (tick <= 9) {
            struct ContactPosition *velocity = (struct ContactPosition *)(position + 20);
            SceneActor_SetChargeOffset(((struct SceneGraphicsActor *)p)->hitAngle, velocity, 24);
        }
        if (tick > 19) {
            base->actionFinished = 1;
            ((struct SceneGraphicsActor *)p)->variant = 0;
        }
    }
}
