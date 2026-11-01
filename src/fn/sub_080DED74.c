#include "global.h"
#include "scene_actor_registry.h"

struct SceneActorActionCooldown {
    u8 unknown0[0x678];
    u32 flags;
    u8 unknown67C[0x14];
    u16 remaining, normalDelay, alternateDelay;
};
static inline u8 Unavailable(struct SceneActorRegistryActor *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if (actor->flags2D4 & mask)
            return 1;
        mask = 0x20000;
        if (actor->flags2D0 & mask)
            return 1;
        if (actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline u8 HasPending(void **slot)
{
    u32 mask = 1;
    if (((struct SceneActorActionCooldown *)*slot)->flags & mask)
        return 1;
    return 0;
}
static inline u32 MaskFlags(u32 value, u32 *p)
{
    return *p & value;
}
static inline void QueueAction(struct SceneActorRegistryActor *actor)
{
    u32 behavior = 15;
    u32 zero;
    u8 *q = &actor->state2A8;
    zero = 0;
    *q = zero;
    actor->phase2A9 = zero;
    actor->behavior2AA = behavior;
    actor->counter2AC = zero;
    actor->field2B1 = 1;
    {
        u32 *flags;
        s32 mask;
        struct SceneActorActionCooldown *ctx;
        ctx = actor->context;
        flags = &ctx->flags;
        mask = -2;
        *flags &= mask;
    }
}
void SceneActorControl_TickActionCooldown(struct SceneActorRegistryActor *actor)
{
    struct SceneActorActionCooldown *cooldown = actor->context;
    u32 flags;
    if (Unavailable(actor))
        return;
    {
        u32 mask = 0xE33F;
        flags = actor->flags2D4;
        if (mask & flags)
            return;
    }
    if (MaskFlags(0x10, &actor->flags2CC))
        return;
    if (actor->unknown2B5[0] > 1)
        return;
    if (cooldown->remaining) {
        cooldown->remaining--;
        return;
    }
    if (flags & 0x80)
        cooldown->remaining = cooldown->alternateDelay;
    else
        cooldown->remaining = cooldown->normalDelay;
    {
        u32 one = 1;
        u32 *p = &cooldown->flags;
        *p |= one;
    }
    if (HasPending(&actor->context)) {
        QueueAction(actor);
    }
}
