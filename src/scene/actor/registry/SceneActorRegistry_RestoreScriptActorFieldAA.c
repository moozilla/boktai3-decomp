#include "scene/actor/scene_actor_registry.h"
#include "engine/word_flags.h"
struct ChaseActor *SceneActorRegistry_FindById(u32);
void SceneActorRegistry_RestoreScriptActorFieldAA(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    if (id) {
        struct ChaseActor *actor = SceneActorRegistry_FindById(id);
        if (actor && actor->group.contactControl) {
            s32 value = actor->base.attachmentRestart;
            actor->base.attachmentTimer = value;
            HalfwordFlags_Set(2, &actor->base.reactionFlags);
        }
    }
}
