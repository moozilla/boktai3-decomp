#include "scene_effect_schedule.h"
void sub_08122DA0(void);
s32 SceneEffectSchedule_Initialize(struct SceneEffectSchedule *manager, u32 unusedArgument)
{
    sub_08122DA0();
    gUnk_0200019C = manager;
    return 0;
}
