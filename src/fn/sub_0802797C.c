#include "scene_actors.h"
#include "scene_graphics_actor.h"

static inline void setFacing(struct StateActor *p, int angle)
{
    p->bodyDirection = angle;
    {
        int shifted = (p->bodyDirection + 0x60) >> 6;
        u32 value = 3;
        value &= shifted;
        p->animationFacing4 = value;
    }
    {
        int shifted = (p->bodyDirection + 0x70) >> 5;
        u32 value = 7;
        value &= shifted;
        p->animationFacing8 = value;
    }
}
static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
void SceneActors_UpdateEightDirectionTurn(struct StateActor *p)
{
    struct StateActor *p2 = p;
    u8 *position = (u8 *)p + 0x48;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        sequence = choose(((struct SceneGraphicsActor *)p)->graphics.animationFlags, 38, 0);
        SceneActors_StartEightDirectionAnimation(
            p, &((struct SceneGraphicsActor *)p)->animation,
            (int)&((struct SceneGraphicsActor *)p)->animationResource, sequence, 1, 1, 4);
    } else {
        int sequence = choose(((struct SceneGraphicsActor *)p)->graphics.animationFlags, 38, 0);
        SceneActors_StartEightDirectionAnimation(
            p, &((struct SceneGraphicsActor *)p)->animation,
            (int)&((struct SceneGraphicsActor *)p)->animationResource, sequence, 1, 1, 1);
    }
    SceneGraphicsActor_ClearPendingDamage((struct SceneGraphicsActor *)p);
    {
        u32 mask = 12;
        p2->actionFlags |= mask;
    }
    if (!p2->actionReset(p2)) {
        int delta = (s8)(position[9] - (u8)p2->savedFacing), distance = delta;
        if (distance < 0)
            distance = -distance;
        if (distance <= 7) {
            setFacing(p2, p2->savedFacing);
            p2->actionFinished = 1;
        } else {
            int angle;
            if (delta < 0)
                angle = position[9] + 8;
            else
                angle = position[9] - 8;
            setFacing(p2, angle);
        }
        ((struct SceneGraphicsActor *)p)->graphics.scaleMin = 0;
        ((struct SceneGraphicsActor *)p)->graphics.scaleMax = 0;
        ((struct SceneGraphicsActor *)p)->variant = 0;
    }
}
