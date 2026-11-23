#include "scene_actors.h"
#include "scene_actor_charge_offset.h"
#include "scene_graphics_actor.h"

static inline int trunc12(int x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
void SceneActors_UpdateExpandingShake(struct StateActor *p, int time)
{
    int tick = time;
    struct StateActor *p2 = p;
    u8 *position = (u8 *)p + 0x48;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        {
            int test = ((struct SceneGraphicsActor *)p)->graphics.animationFlags;
            test &= 1;
            sequence = 43;
            if (test)
                sequence = 5;
        }
        SceneActors_StartDirectionalAnimation(
            p, &((struct SceneGraphicsActor *)p)->animation,
            (int)&((struct SceneGraphicsActor *)p)->animationResource, sequence, 1, 1, 4);
        p->targetPosition = *(struct StateActorPos *)(position + 12);
    }
    SceneGraphicsActor_ClearPendingDamage((struct SceneGraphicsActor *)p2);
    SceneActorState_SetFlags(p, 0x1000c);
    if (!p->actionReset(p)) {
        *(struct StateActorPos *)(position + 12) = p->targetPosition;
        {
            int dir = tick & 3;
            struct ContactPosition *velocity = (struct ContactPosition *)(position + 20);
            u32 angle;
            int speed;
            angle = dir << 6;
            speed = tick << 3;
            {
                int x = trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
                velocity->x = x;
                velocity->y = 0;
            }
            {
                const s16 *entry = &gUnk_086149C4[angle];
                velocity->z = trunc12(*entry * speed);
            }
        }
        if (tick > 31)
            p->actionFinished = 1;
    }
}
