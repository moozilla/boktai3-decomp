#include "global.h"

/* Dog_RestoreRoute: unless flags2D4 bit 0x100 is set, restores after a delay, records the follow-up control (chase when flags2D4 & 0x80, else the entry target) and routes back toward the chased object via the route point (provisional name) */
#include "scene_actor_chase.h"

static inline void SaveDelay(u8 *saved, void *target, u32 behavior, u32 delay)
{
    *(u32 *)(saved + 0xC) = delay;
    *(void **)(saved + 0x10) = target;
    saved[8] = 0;
    saved[9] = 0;
    saved[10] = behavior;
}

// confidence=low: dog cohort variant of the shared delayed restoration routine.
void Dog_RestoreRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    u16 *flags;
    u32 chasing;

    {
        unsigned short mask = 0x100;
        flags = &actor->flags2D4;
        if ((u16)((*flags) & mask)) {
            return;
        }
    }
    Chase_SetState(actor, 3, 1, 0);
    Chase_SetCtl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
    chasing = 0x80;
    if (*flags & chasing) {
        actor->field2C2 = 0x20;
        SaveDelay(group + 0x8C, *(void **)(group + 0x48), 0xC, 0x14);
        if (*flags & chasing) {
            u16 *time = (u16 *)((u8 *)actor + 0x418);
            *time += 0x12C;
            if (*time > *(u16 *)((u8 *)actor + 0x416))
                *time = *(u16 *)((u8 *)actor + 0x416);
        }
        {
            u16 *field = (u16 *)(group + 0x40);
            u32 mask = 4;
            u32 value = *field;
            mask |= value;
            *field = mask;
        }
    } else {
        {
            u32 mask = 0x10;
            actor->flags2CC |= mask;
        }
        actor->field2C2 = 0x20;
        SaveDelay(group + 0x8C, actor->entryTarget228, 7, 0x3C);
    }
    Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
    (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position,
                        &((struct ChaseTarget *)actor->tracking.target)->position))
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
}
