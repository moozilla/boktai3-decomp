#include "scene_actor_chase.h"
u32 Slime_GetRandomVariantFlags(u8 *p);
struct SlimeHitInfo {
    u8 unknown0[4];
    u16 value;
    u8 unknown6[4];
    u16 flags;
};
struct SlimeHitTarget {
    u8 unknown0[0x38];
    u32 variantFlags;
};
static inline u8 Slime_Unavailable(struct SceneActorRegistryResetView *actor)
{
    if (!actor)
        goto zero;
    {
        u32 mask;
        u32 offset;
        u32 *field;
        u32 value;
        mask = 0x40;
        offset = 0x2d4;
        field = (void *)((u8 *)actor + offset);
        value = *(u16 *)field;
        value &= mask;
        if (value)
            goto one;
        mask = 0x20000;
        offset -= 4;
        field = (void *)((u8 *)actor + offset);
        value = *field;
        value &= mask;
        if (value)
            goto one;
    }
    if (actor->health > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}
void Slime_ApplyBodyHit(struct SlimeHitTarget *target, struct SlimeHitInfo *info,
                        struct SceneActorRegistryResetView *actor)
{
    s32 k2 = 1;
    if ((!Slime_Unavailable(actor)) && (info->flags == k2)) {
        if (!Slime_Unavailable(actor) && (info->flags & 0xF)) {
            actor->field304 = info->value;
            {
                u32 *field = &actor->flags2D0;
                u32 mask = ~0x1000000;
                *field &= mask;
            }
        }
        if ((*((u16 *)(((u8 *)actor) + 0x1E6))) == 3) {
            target->variantFlags = Slime_GetRandomVariantFlags((u8 *)actor);
        }
    }
}
