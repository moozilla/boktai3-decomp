// MATCHING_CONCESSION: register-pinning
#include "scene_actor_script.h"
static inline void KeepFlags(u32 mask, u32 *field)
{
    *field = mask & *field;
}
static inline void SetFlags(u32 mask, u32 *field)
{
    *field = mask | *field;
}
void SceneActorScript_SetFlagFromScript(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    struct SceneActorRegistryActor *actor;
    if (!id)
        return;
    actor = SceneActorRegistry_FindById(id);
    if (!actor)
        return;
    if (!Script_GetKeywordValue('f', 0)) {
        register u32 *flags asm("r2") = &actor->flags2D0;
        asm("" : "+r"(flags));
        KeepFlags(~0x04000000, flags);
    } else
        SetFlags(0x04000000, &actor->flags2D0);
}
