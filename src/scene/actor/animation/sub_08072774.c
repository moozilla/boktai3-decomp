#include "scene/actor/scene_actor_script.h"
#include "engine/word_flags.h"
static inline void SetFlags(u32 mask, u32 *field)
{
    *field = mask | *field;
}
void SceneActorScript_SetFlagFromScript(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    struct ChaseActor *actor;
    if (!id)
        return;
    actor = SceneActorRegistry_FindById(id);
    if (!actor)
        return;
    if (!Script_GetKeywordValue('f', 0)) {
        u32 *flags = &actor->base.statusFlags;
        WordFlags_And(~0x04000000, flags);
    } else
        SetFlags(0x04000000, &actor->base.statusFlags);
}
