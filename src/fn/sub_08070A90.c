// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorRegistry_SetMovementMode(struct SceneActorRegistryActor *, s32);
static inline u32 ReadExclusionFlags(struct SceneActorRegistryActor *actor)
{
    u32 mask = 0x20000;
    u32 offset = 0x2d0;
    u32 *field;
    u32 value;
    field = (void *)((u8 *)actor + offset);
    value = *field;
    value &= mask;
    return value;
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
            u32 unavailable;
            {
                register u16 *field asm("r1");
                u32 value;
                register u32 mask asm("r3");
                {
                    register u32 offset asm("r2") = 0x2d4;
                    asm("" : "+r"(offset));
                    field = (void *)((u8 *)actor + offset);
                }
                value = *field;
                mask = disabled;
                asm("" : "+r"(mask));
                value &= mask;
                flags = field;
                if (value || ReadExclusionFlags(actor) || actor->health <= 0)
                    unavailable = 1;
                else
                    unavailable = 0;
            }
            asm("" : "+r"(unavailable));
            if (!unavailable && (*flags & enabled))
                SceneActorRegistry_SetMovementMode(actor, mode);
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
}
