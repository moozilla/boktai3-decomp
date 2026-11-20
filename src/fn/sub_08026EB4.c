#include "scene_actors.h"
#include "scene_actor_charge_offset.h"
#include "scene_graphics_actor.h"

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
    {
        u32 mask = 12;
        base->actionFlags |= mask;
    }
    if (((struct SceneGraphicsActor *)p)->graphics.scaleMin > 23)
        ((struct SceneGraphicsActor *)p)->graphics.scaleMin -= 24;
    else
        ((struct SceneGraphicsActor *)p)->graphics.scaleMin = 0;
    decay(&((struct SceneGraphicsActor *)p)->graphics.scaleMax);
    if (!base->actionReset(base)) {
        if (tick <= 9) {
            u8 *anglePtr = &((struct SceneGraphicsActor *)p)->hitAngle;
            struct ContactPosition *velocity = (struct ContactPosition *)(position + 20);
            u32 angle;
            int speed;
            {
                u32 value = *anglePtr;
                angle = value;
            }
            speed = 24;
            SceneActor_SetChargeOffset(angle, velocity, speed);
        }
        if (tick > 19) {
            u32 z;
            u8 *dst = &base->actionFinished;
            z = 0;
            *dst = 1;
            ((struct SceneGraphicsActor *)p)->variant = z;
        }
    }
}
