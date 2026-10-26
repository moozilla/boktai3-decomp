#include "scene_effect_schedule.h"
void SceneScheduledEffect_EnsureSingleton(void);
s32 SceneEffectSchedule_Initialize(struct SceneEffectSchedule *manager, u32 unusedArgument)
{
    SceneScheduledEffect_EnsureSingleton();
    gSceneEffectSchedule = manager;
    return 0;
}
