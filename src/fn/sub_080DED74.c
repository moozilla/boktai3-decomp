#include "global.h"
#include "scene_actor_registry.h"

struct SceneActorActionCooldown {
    u8 unknown0[0x678];
    u32 flags;
    u8 unknown67C[0x14];
    u16 remaining, normalDelay, alternateDelay;
};
static inline u8 Unavailable(struct ChaseActor *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if (actor->base.flags2D4 & mask)
            return 1;
        mask = 0x20000;
        if (actor->base.flags2D0 & mask)
            return 1;
        if (actor->base.health <= 0)
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
static inline void QueueAction(struct ChaseActor *actor)
{
    u32 behavior = 15;
    u32 zero;
    u8 *q = &actor->base.state2A8;
    zero = 0;
    *q = zero;
    actor->base.phase2A9 = zero;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = zero;
    actor->base.field2B1 = 1;
    {
        u32 *flags;
        s32 mask;
        struct SceneActorActionCooldown *ctx;
        ctx = actor->base.context;
        flags = &ctx->flags;
        mask = -2;
        *flags &= mask;
    }
}
void SceneActorControl_TickActionCooldown(struct ChaseActor *actor)
{
    struct SceneActorActionCooldown *cooldown = actor->base.context;
    u32 flags;
    if (Unavailable(actor))
        return;
    {
        u32 mask = 0xE33F;
        flags = actor->base.flags2D4;
        if (mask & flags)
            return;
    }
    if (MaskFlags(0x10, &actor->base.flags2CC))
        return;
    if (actor->base.field2B5 > 1)
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
    if (HasPending(&actor->base.context)) {
        QueueAction(actor);
    }
}
