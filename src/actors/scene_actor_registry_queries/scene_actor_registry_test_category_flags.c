#include "engine/shared/scene_actor_registry.h"
struct ChaseActor *SceneActorRegistry_GetFirstActor(void);
struct ChaseActor *SceneActorRegistry_FindAvailableAfterId(u32);
static inline u8 IsUnavailable(struct ChaseActor *actor, u32 disabled, u32 excluded)
{
    if (actor) {
        if ((actor->base.motionFlags & disabled) || (actor->base.statusFlags & excluded) ||
            actor->base.health <= 0)
            return 1;
    }
    return 0;
}

u32 SceneActorRegistry_TestCategoryFlags(s32 inputCategory, u32 inputFlags)
{
    s32 category = inputCategory;
    u32 flags = inputFlags;
    struct ChaseActor *actor = SceneActorRegistry_GetFirstActor();
    if (actor)
        goto initialize;
    goto failure;
success:
    return 1;
initialize: {
    u32 disabled = 0x40;
    u32 excluded = 0x20000;
    do {
        if (actor->base.category == category) {
            u32 unavailable = IsUnavailable(actor, disabled, excluded);
            if (!unavailable && (actor->base.behaviorFlags & flags))
                goto success;
        }
        actor = SceneActorRegistry_FindAvailableAfterId(actor->base.body.id);
    } while (actor);
}
failure:
    return 0;
}
