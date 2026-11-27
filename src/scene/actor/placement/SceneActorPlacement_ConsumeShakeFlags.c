#include "scene/motion/scene_actor_availability.h"
#include "scene/actor/scene_actor_placement.h"

static inline void ShakeDrawable(struct RoomActor *actor, s32 span)
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
    table = gRandomTable;
    index = &gRandomIndex;
    {
        u32 next = *index + 1;
        mask = 1023;
        *index = next & mask;
    }
    value = Mod(table[*index], span);
    coordinate = (u16)position->x;
    coordinate -= span / 2;
    coordinate += value;
    position->x = coordinate;
    *index = (*index + 1) & mask;
    value = Mod(table[*index], span);
    coordinate = (u16)position->z;
    coordinate -= span / 2;
    coordinate += value;
    position->z = coordinate;
}

u32 SceneActorPlacement_ConsumeShakeFlags(struct ChaseActor *argument)
{
    struct RoomActor *actor = (void *)argument;
    u32 unavailable = SceneActor_ActionsBlocked((void *)actor);
    if (!unavailable && actor->reactionFlags) {
        u8 small;
        u16 *flags;
        small = SceneActor_TestPendingFlag((void *)actor, 0x10, &flags);
        if (small) {
            ShakeDrawable(actor, 10);
            SceneActorPlacement_ClearFlags(flags, ~0x10);
        } else if (SceneActorPlacement_TestFlags(flags, 0x20)) {
            ShakeDrawable(actor, 16);
            SceneActorPlacement_ClearFlags(flags, ~0x20);
        } else
            return 0;
        return 1;
    }
    return 0;
}
