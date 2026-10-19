
#ifdef NONMATCHING
// fuzzy: 87.6%
// foundry draft (worker best 87.5%); what differs is mostly register allocation and instruction order
#define SceneActorControl_TickActionCooldown sub_080DED74
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
static inline u8 HasPending(struct SceneActorRegistryActor *actor)
{
    u32 mask = 1;
    if (((struct SceneActorActionCooldown *)actor->context)->flags & mask)
        return 1;
    return 0;
}
static inline u32 MaskFlags(u32 value, u32 *p)
{
    return *p & value;
}
static inline void QueueAction(u8 *actor)
{
    u32 behavior = 15;
    u32 offset = 0x2A8;
    u32 zero = 0;
    u32 odd;
    ((u8 *)actor)[offset] = zero;
    odd = 0x2A9;
    ((u8 *)actor)[odd] = zero;
    offset += 2;
    ((u8 *)actor)[offset] = behavior;
    *(u32 *)(actor + 0x2AC) = zero;
    odd += 8;
    ((u8 *)actor)[odd] = 1;
    {
        u32 *flags = &(*(struct SceneActorActionCooldown **)(actor + 0x3D0))->flags;
        s32 mask = -2;
        *flags &= mask;
    }
}
void sub_080DED74(struct SceneActorRegistryActor *actor)
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
    if (HasPending(actor)) {
        QueueAction((u8 *)actor);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080DED74);
#endif
