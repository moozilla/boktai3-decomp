#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorRegistry_SetMovementMode(struct SceneActorRegistryActor *, s32);
static inline u8 IsUnavailable(struct SceneActorRegistryActor *actor, u16 **saved, u32 disabled)
{
    u16 *flags = &actor->flags2D4;
    u32 value = *flags;
    u32 mask = disabled;
    value &= mask;
    *saved = flags;
    if (value)
        goto one;
    {
        u32 value;
        mask = 0x20000;
        value = actor->flags2D0;
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
void SceneActorRegistry_SetMovementModesFromScript(void)
{
    struct SceneActorRegistryNode *node;
    struct SceneActorRegistryActor *actor;
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
