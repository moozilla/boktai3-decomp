#include "scene_actor_control_phase.h"
void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorView *);
static inline u8 Unavailable(u8 *c)
{
    u32 mask;
    u32 *p;
    u32 off;
    if (!c)
        goto zero;
    mask = 0x40;
    off = 0xB5;
    off <<= 2;
    p = (u32 *)(c + off);
    if ((*((u16 *)p)) & mask)
        goto one;
    mask = 0x80;
    mask <<= 10;
    off -= 4;
    p = (u32 *)(c + off);
    if ((*p) & mask)
        goto one;
    if ((*((s16 *)(c + 0x9C))) > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}
void Crow_OnBodyContact(struct SceneActorBehaviorContactView *source,
                        struct SceneActorBehaviorContactView *destination,
                        struct SceneActorRegistryResetView *actor)
{
    u32 index;
    s32 power;
    u32 sample;
    if (Unavailable((u8 *)actor)) {
        return;
    }
    {
        u32 mask = 0x800;
        if (source->flags & mask) {
            return;
        }
    }
    SceneActorBehavior_ApplyContactReaction(source, destination, (void *)actor);
    index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    {
        const u16 *table = (const u16 *)0x0203B400;
        sample = *(u16 *)(index + (u32)table);
    }
    {
        s32 old = destination->power;
        u32 mask = 15;
        power = old - (sample & mask);
    }
    power = power < 20 ? 20 : power;
    destination->power = power;
}
