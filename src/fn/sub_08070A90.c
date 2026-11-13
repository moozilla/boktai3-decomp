#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorRegistry_SetMovementMode(struct SceneActorRegistryActor *, s32);
static inline u32 ReadExclusionFlags(struct SceneActorRegistryActor *actor)
{
    u32 mask = 0x20000;
    u32 *field;
    u32 value;
    field = &actor->flags2D0;
    value = *field;
    value &= mask;
    return value;
}
static inline u8 IsUnavailable(struct SceneActorRegistryActor *actor, u16 **saved, u32 disabled)
{
    u16 *flags = &actor->flags2D4;
    u32 value = *flags;
    u32 mask = disabled;
    value &= mask;
    *saved = flags;
    if (value || ReadExclusionFlags(actor) || actor->health <= 0)
        return 1;
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
    nextActor: {
        u16 *flags;
        u32 unavailable;
        unavailable = IsUnavailable(actor, &flags, disabled);
        if (!unavailable && (*flags & enabled))
            SceneActorRegistry_SetMovementMode(actor, mode);
        node = node->next;
        actor = node->actor;
        if (actor)
            goto nextActor;
    }
    }
}
