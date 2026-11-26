#include "scene/actor/scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorRegistry_SetMovementMode(struct ChaseActor *, s32);
static inline u8 IsUnavailable(struct ChaseActor *actor, u16 **saved, u32 disabled)
{
    u16 *flags = &actor->base.motionFlags;
    u32 value = *flags;
    u32 mask = disabled;
    value &= mask;
    *saved = flags;
    if (value)
        goto one;
    {
        u32 value;
        mask = 0x20000;
        value = actor->base.statusFlags;
        value &= mask;
        if (value)
            goto one;
    }
    if (actor->base.health > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}
void SceneActorRegistry_SetMovementModesFromScript(void)
{
    struct SceneActorRegistryNode *node;
    struct ChaseActor *actor;
    s32 mode;
    if (!SceneActorRegistry_GetManager())
        return;
    node = gSceneActorRegistryNode;
    actor = node->actor;
    mode = Script_GetKeywordValue('d', 0);
    if (actor) {
        u32 disabled = 0x40;
        u32 enabled = 0x4000;
        do {
            u16 *flags;
            u32 unavailable = IsUnavailable(actor, &flags, disabled);
            if (!unavailable && (*flags & enabled))
                SceneActorRegistry_SetMovementMode(actor, mode);
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
}
