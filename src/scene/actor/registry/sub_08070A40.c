#include "scene/actor/scene_actor_registry.h"
#include "scene/actor/scene_actor_availability.h"
struct ChaseActor *SceneActorRegistry_FindById(u32);
void SceneActorRegistry_SetMovementMode(struct ChaseActor *, s32);
static inline u8 HasMovementFlag(struct ChaseActor *actor)
{
    if (SceneActor_MaskedHalfword(0x100, &actor->base.placementFlags))
        return 1;
    return 0;
}
void SceneActorRegistry_SetScriptActorMovementMode(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    s32 mode = Script_GetKeywordValue('d', 0);
    if (id) {
        struct ChaseActor *actor = SceneActorRegistry_FindById(id);
        if (actor) {
            u32 enabled = HasMovementFlag(actor);
            if (enabled)
                SceneActorRegistry_SetMovementMode(actor, mode);
        }
    }
}
