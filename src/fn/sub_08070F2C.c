#include "scene_actor_registry.h"
struct SceneActorRegistryActor *SceneActorRegistry_GetFirstActor(void);
struct SceneActorRegistryActor *SceneActorRegistry_FindAvailableAfterId(u32);
static inline u8 IsUnavailable(struct SceneActorRegistryActor *actor, u32 disabled, u32 excluded)
{
    if (actor) {
        if ((actor->flags2D4 & disabled) || (actor->flags2D0 & excluded) || actor->health <= 0)
            return 1;
    }
    return 0;
}

u32 SceneActorRegistry_TestCategoryFlags(s32 inputCategory, u32 inputFlags)
{
    s32 category = inputCategory;
    u32 flags = inputFlags;
    struct SceneActorRegistryActor *actor = SceneActorRegistry_GetFirstActor();
    if (actor)
        goto initialize;
    goto failure;
success:
    return 1;
initialize: {
    u32 disabled = 0x40;
    u32 excluded = 0x20000;
    do {
        if (actor->category == category) {
            u32 unavailable = IsUnavailable(actor, disabled, excluded);
            if (!unavailable && (actor->flags2CC & flags))
                goto success;
        }
        actor = SceneActorRegistry_FindAvailableAfterId(actor->id);
    } while (actor);
}
failure:
    return 0;
}
