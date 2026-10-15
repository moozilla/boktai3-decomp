#include "scene_actor_control.h"
#define SceneActorControl_ResetSingleton sub_08067984
extern struct SceneActorControlManager *gUnk_02000128;
void SceneActorControl_ResetSingleton(void)
{
    gUnk_02000128 = 0;
}
