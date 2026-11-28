#include "scene/actor/scene_actor_script.h"
static inline s32 ReadOrZero(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}
void SceneActorScript_SetControlBytesFromScript(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    struct SceneActorScriptPropertiesView *actor;
    struct SceneActorScriptControlView *control;
    s32 first, second;
    if (!id)
        return;
    actor = (void *)SceneActorRegistry_FindById(id);
    if (!actor)
        return;
    control = &actor->control;
    if (!Script_SeekToKeyword('r'))
        return;
    first = ReadOrZero();
    second = ReadOrZero();
    control->parameter0 = first;
    control->parameter1 = second;
}
