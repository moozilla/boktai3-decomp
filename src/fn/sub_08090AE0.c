#include "global.h"
#include "scene_actor_motion.h"
#define SceneActorMotion_ArmPrimaryCollider sub_08090AE0

void sub_08090AE0(struct SceneActorMotionView *actor)
{
    s32 health;

    actor->effectDirection = 0;
    actor->payload2E0 = 1;
    health = actor->health;
    actor->primaryCollider.cost = health;
    actor->result2E4 = health;
    {
        u16 *field = (void *)((u8 *)&actor->primaryCollider + 0x40);
        *field = 40;
    }
    actor->parameter2D8 = 0;
    actor->field2DC = 0;
    actor->mode2E6 = 1;
    {
        register u32 mask = 0x2000;
        u32 *flags;
        flags = &actor->flags2CC;
        *flags |= mask;
    }
}
