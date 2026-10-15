#include "scene_actor_script.h"
#define SceneActorScript_SetControlBytesFromScript sub_08072264
static inline s32 ReadOrZero(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}
void SceneActorScript_SetControlBytesFromScript(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    struct SceneActorRegistryActor *actor;
    struct SceneActorScriptControlView *control;
    s32 first, second;
    if (!id)
        return;
    actor = sub_08070EBC(id);
    if (!actor)
        return;
    control = (void *)((u32)actor + 0x3D4);
    if (!Script_SeekToKeyword('r'))
        return;
    first = ReadOrZero();
    second = ReadOrZero();
    control->parameter0 = first;
    control->parameter1 = second;
}
