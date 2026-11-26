#include "actors/common_scene_actor_controller/scene_actors.h"
#include "engine/shared/scene_actor_charge_offset.h"
#include "engine/shared/scene_graphics_actor.h"

typedef struct {
    u32 a;
    u32 b;
} Point;
void SceneActors_UpdateContractingShake(struct StateActor *p, int time)
{
    int tick = time;
    struct StateActor *p2 = p;
    u8 *position = ((u8 *)p) + 0x48;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        {
            int test = ((struct SceneGraphicsActor *)p)->graphics.animationFlags;
            test &= 1;
            sequence = 43;
            if (test) {
                sequence = 5;
            }
        }
        SceneActors_StartDirectionalAnimation(
            p, &((struct SceneGraphicsActor *)p)->animation,
            (int)&((struct SceneGraphicsActor *)p)->animationResource, sequence, 1, 1, 4);
        SceneActorState_SetFlags(p, 16);
        *((Point *)(((u8 *)p) + 0xd0)) = *((Point *)(position + 12));
    }
    SceneGraphicsActor_ClearPendingDamage((struct SceneGraphicsActor *)p2);
    {
        u32 mask = 0x1000c;
        /* both arms are the same; the split keeps agbcc's original block layout */
        if (tick) {
            p->actionFlags |= mask;
        } else {
            p->actionFlags |= mask;
        }
    }
    if (!p->actionReset(p)) {
        *((Point *)(position + 12)) = *((Point *)(((u8 *)p) + 0xd0));
        if (tick > 31) {
            p->actionFinished = 1;
        } else {
            int dir = 3 & (~tick);
            int rate = 32 - tick;
            struct ContactPosition *velocity = (struct ContactPosition *)(position + 20);
            u32 angle;
            int speed;
            angle = dir << 6;
            speed = rate << 3;
            SceneActor_SetChargeOffset(angle, velocity, speed);
        }
    }
}
