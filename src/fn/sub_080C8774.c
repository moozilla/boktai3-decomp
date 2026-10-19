#include "global.h"
#ifdef NONMATCHING
// fuzzy: 73.0%
// Golem push/settle state, sibling of 080BBDD0: decays the push offset into motion, then chases
// the player (two Chase_Arrive copies, include/scene_actor_chase.h). The not-settled
// flags2CC |= 0x2000 is the shared tail at 080C9824.
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

void Sound_PlaySong(u32);

void sub_080C8774(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    s16 *pushX, *pushZ;
    s32 dx, dz;
    u16 flags;

    if (TakeFlag2B2(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        if (actor->field2D8 & 0x400)
            Sound_PlaySong(0x18B);
        actor->flags2D4 = (actor->flags2D4 | 0x20) & ~0x100;
    }
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
    if (!IsSettled(dx, dz) || !actor->field2B3)
        goto busy;
    actor->field2B0 = 1;
    flags = actor->flags2D4 & ~0x20;
    actor->flags2D4 = flags;
    if (*(u32 *)((u8 *)actor + 0x2DC) & 0x80000) {
        Chase_SetCtl(actor, *(void **)(group + 0x208), 7);
        Chase_SetState(actor, 3, 0, 0);
        return;
    }
    if (flags & 0x80) {
        actor->flags2CC |= 0x80;
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
        return;
    }
    actor->fieldDC = (u32)((u8 *)gUnk_02000580[0] + 0x24);
    actor->flags2CC |= 0x90;
    (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    if (!Chase_Arrive(actor) &&
        !Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                        &((struct ChaseTarget *)actor->fieldDC)->position)) {
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
        return;
    }
    Chase_SetCtl(actor, actor->entryTarget228, 1);
    Chase_SetBehavior(actor, 7);
    actor->field2C2 = 0x20;
    return;
busy:
    actor->flags2CC |= 0x2000;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080C8774);
#endif
