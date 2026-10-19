#include "global.h"
#ifdef NONMATCHING
// fuzzy: 59.8%
// Golem chase state. The chase (SceneActorChase_Pursue, include/scene_actor_chase.h) is
// shared with 080BBDD0/080BF0E4/080BFCC4/080C12FC. Still differs: the ROM copies the group
// pointer into sl for the turn helper, keeps a zero in r8 for the byte stores, and keeps the
// outer actor in r6 (mine swaps r6/r7). Steer by sbs-style register-masked diffs.
#include "scene_actor_chase.h"

void sub_080C09F0(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    u8 *group = (u8 *)&actor->group;

    if (SceneActorChase_TakeFlag2B1(actor)) {
        void *target = Skeleton_UpdateScriptWaitState;
        SceneActorChase_SetControl(actor, 0x26, target);
    }
    if (**(u8 **)((u8 *)actor + 0x6C) & 1) {
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 0xC;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
        SceneActorChase_SetControl(actor, 0x1A, *(void **)(group + 0x48));
        SceneActorChase_Pursue(actor);
        *(u32 *)((u8 *)actor->context + 0x6DC) |= 0x80;
    } else {
        struct ContactPosition delta;
        u32 angle;
        s32 speed;
        SceneActorTerrain_Subtract(&delta, (struct ContactPosition *)((u8 *)actor->fieldDC + 0xC),
                                   &actor->position);
        angle = Math_Atan2(delta.x, delta.z);
        if (actor->distanceF0 < *(u32 *)(body + 0x708)) {
            angle = (angle + 0x80) & 0xFF;
            actor->field2C2 = 0x20;
        } else if (actor->distanceF0 > *(u32 *)(body + 0x70C)) {
            actor->field2C2 = 0x20;
            actor->direction = angle;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        speed = Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x260));
        if (actor->flags2D4 & 0xA02F)
            actor->field2C2 = 0x40;
        else if (actor->field2C2 == 0)
            actor->field2C2 = 0x40;
        if (actor->drawableKind == 0)
            Anim_SetDurationScale((u8 *)actor->drawable + 0x48, actor->field2C2);
        else
            Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, actor->field2C2);
        actor->motion.x += SceneActorChase_Scale16(gUnk_086149C4[(angle + 0x40) & 0xFF] * speed);
        actor->motion.z += SceneActorChase_Scale16(gUnk_086149C4[angle & 0xFF] * speed);
        actor->flags2CC |= 0x2000;
        if (actor->distanceF0 > *(u32 *)(body + 0x708) &&
            actor->distanceF0 < *(u32 *)(body + 0x70C))
            actor->flags2D6 |= 0x80;
        else
            actor->counter2AC++;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080C09F0);
#endif
