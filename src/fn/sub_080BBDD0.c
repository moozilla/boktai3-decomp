#include "global.h"
#ifdef NONMATCHING
// fuzzy: 67.2%
// Push/settle state: decays the push offset at +0x318/+0x31C into motion, then (once settled)
// chases the player through two copies of Chase_Arrive (include/scene_actor_chase.h), falling
// back to Chase_TryRoute toward the chased object.
#include "scene_actor_chase.h"

static inline u8 TakeFlag2B2(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2) {
        actor->field2B2 = 0;
        return 1;
    }
    return 0;
}
static inline s32 Scale4(s32 n)
{
    return n >= 0 ? n >> 2 : -((-n) >> 2);
}
static inline u8 IsSettled(s32 dx, s32 dz)
{
    if (dx == 0 && dz == 0)
        return 1;
    return 0;
}

void sub_080BBDD0(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    s16 *pushX, *pushZ;
    s32 dx, dz;
    u8 *body;
    u32 *flags2CC;
    u32 flags;

    if (TakeFlag2B2(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
    actor->flags2CC |= 0x2000;
    actor->flags2D0 |= 0x10000;
    pushX = (s16 *)((u8 *)actor + 0x318);
    dx = Scale4(*(s16 *)((u8 *)actor + 0x310) - *pushX);
    pushZ = (s16 *)((u8 *)actor + 0x31C);
    dz = Scale4(*(s16 *)((u8 *)actor + 0x314) - *pushZ);
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->motion.x += dx;
    actor->motion.z += dz;
    *pushX += dx;
    *pushZ += dz;
    if (!IsSettled(dx, dz))
        return;
    body = actor->context;
    actor->field2B0 = 1;
    actor->flags2D4 &= ~0x120;
    flags2CC = &actor->flags2CC;
    flags = *flags2CC & ~0x80;
    *flags2CC = flags;
    if (*(u32 *)((u8 *)actor + 0x2DC) & 0x80000) {
        actor->flags2D6 |= 2;
        return;
    }
    if (Chase_MotionFlags(actor, 0x80)) {
        *flags2CC = flags | 0x80;
        *(u16 *)(group + 0x40) |= 4;
        actor->fieldDC = (u32)((u8 *)gUnk_02000580[0] + 0x24);
        if (!Chase_Arrive(actor) &&
            !Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                            &((struct ChaseTarget *)actor->fieldDC)->position)) {
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            return;
        }
        Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
        Chase_SetBehavior(actor, 12);
    } else {
        actor->fieldDC = (u32)((u8 *)gUnk_02000580[0] + 0x24);
        *flags2CC = flags | 0x10;
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        if (!Chase_Arrive(actor)) {
            *(u16 *)(body + 0x6F0) = 0;
            if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                                &((struct ChaseTarget *)actor->fieldDC)->position)) {
                Callback_InvokeActor(actor, *(void **)(group + 0x224));
                return;
            }
        }
        Chase_SetCtl(actor, actor->entryTarget228, 1);
        Chase_SetBehavior(actor, 7);
        actor->field2C2 = 0x20;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080BBDD0);
#endif
