// MATCHING_CONCESSION: register-pinning
#include "scene_actor_placement.h"
static inline u8 SceneActorPlacement_TestAndSaveFlags(u32 mask, u16 *source, u16 **saved)
{
    u32 bits = *source & mask;
    *saved = source;
    if (bits)
        return 1;
    return 0;
}
u32 SceneActorPlacement_ConsumeShakeFlags(struct SceneActorRegistryActor *argument)
{
    struct SceneActorPlacementView *actor = (void *)argument;
    u8 unavailable;
    if (actor) {
        u32 mask = 0x40;
        if ((actor->flags2D4 & mask) || SceneActorPlacement_ReadFlags32(actor, 0x20000) ||
            actor->health <= 0)
            unavailable = 1;
        else
            unavailable = 0;
    } else
        unavailable = 0;
    asm("" : "+r"(unavailable));
    if (!unavailable && actor->flags2D6) {
        u8 small;
        u16 *flags;
        u32 mask = 0x10;
        u16 *source;
        asm("" : "+r"(actor) : "r"(mask));
        source = &actor->flags2D6;
        small = SceneActorPlacement_TestAndSaveFlags(mask, source, &flags);
        if (small) {
            {
                struct ContactPosition *position;
                const u16 *table;
                u32 *index;
                u32 mask;
                s32 value;
                u32 coordinate;
                if (!actor->drawableKind)
                    position =
                        &((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->position;
                else
                    position =
                        &((struct SceneActorRegistryTileDrawable *)actor->drawable)->position;
                table = gUnk_0203B400;
                index = &gRandomIndex;
                {
                    u32 next = *index + 1;
                    mask = 1023;
                    *index = next & mask;
                }
                value = Mod(table[*index], 10);
                coordinate = (u16)position->x;
                coordinate -= 5;
                coordinate += value;
                position->x = coordinate;
                *index = (*index + 1) & mask;
                value = Mod(table[*index], 10);
                coordinate = (u16)position->z;
                coordinate -= 5;
                coordinate += value;
                position->z = coordinate;
            }
            SceneActorPlacement_ClearFlags(flags, ~0x10);
        } else if (SceneActorPlacement_TestFlags(flags, 0x20)) {
            {
                struct ContactPosition *position;
                const u16 *table;
                u32 *index;
                u32 mask;
                s32 value;
                u32 coordinate;
                if (!actor->drawableKind)
                    position =
                        &((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->position;
                else
                    position =
                        &((struct SceneActorRegistryTileDrawable *)actor->drawable)->position;
                table = gUnk_0203B400;
                index = &gRandomIndex;
                {
                    u32 next = *index + 1;
                    mask = 1023;
                    *index = next & mask;
                }
                value = Mod(table[*index], 16);
                coordinate = (u16)position->x;
                coordinate -= 8;
                coordinate += value;
                position->x = coordinate;
                *index = (*index + 1) & mask;
                value = Mod(table[*index], 16);
                coordinate = (u16)position->z;
                coordinate -= 8;
                coordinate += value;
                position->z = coordinate;
            }
            SceneActorPlacement_ClearFlags(flags, ~0x20);
        } else
            return 0;
        return 1;
    }
    return 0;
}
