// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
void SceneActorBehavior_UpdateColliders(struct SceneActorRegistryActor *);
static inline u32 IsUnavailable(struct SceneActorRegistryActor *actor)
{
    u32 result = 0;
    asm("" : "+r"(result));
    if (!actor)
        goto end;
    {
        u32 mask = 0x40;
        u32 offset = 0x2d4;
        u32 value;
        value = *(u16 *)((u8 *)actor + offset);
        if (value & mask)
            goto one;
        mask = 0x20000;
        offset -= 4;
        {
            u32 *field = (void *)((u8 *)actor + offset);
            value = *field;
        }
        if (value & mask)
            goto one;
        if (actor->health > 0)
            goto clear;
    one:
        result = 1;
        goto end;
    }
clear:
    result = 0;
end:
    return result;
}
s32 SceneActorMovement_RequestAvailableAction(struct SceneActorRegistryActor *actor)
{
    u32 unavailable;
    /* The unconditional first store requires a valid actor. */
    *(u32 *)((u8 *)actor + 0x304) = 0;
    unavailable = IsUnavailable(actor);
    asm("" : "+r"(unavailable));
    {
        u32 result;
        if (!unavailable) {
            SceneActorBehavior_UpdateColliders(actor);
            result = 1;
        } else {
            result = 0;
        }
        return result;
    }
}