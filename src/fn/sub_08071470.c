#include "scene_actor_registry.h"
extern const u8 gUnk_08605C18[];
u8 SceneActorRegistry_TestNearestPlayerBounds(struct SceneActorRegistryActor *, const void *);
static inline u8 SceneActorRegistry_IsPaused(void)
{
    if (gEngineFlags & 0x4800)
        return 1;
    return 0;
}
void SceneActorRegistry_AdvanceInactiveActors(struct SceneActorRegistryManager *unusedManager)
{
    if (SceneActorRegistry_IsPaused()) {
        struct SceneActorRegistryNode *node = gUnk_020004AC;
        if (node) {
            struct SceneActorRegistryActor *actor = node->actor;
            if (actor) {
                u32 one = 1;
                do {
                    node = node->next;
                    if (actor->health <= 0 && (actor->flags2CC & one) &&
                        !SceneActorRegistry_TestNearestPlayerBounds(actor, gUnk_08605C18))
                        SceneActorRegistry_RestoreActorDefaults(actor);
                    actor = node->actor;
                } while (actor);
            }
        }
    }
}
