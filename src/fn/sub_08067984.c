#include "scene_actor_control.h"
extern struct SceneActorControlManager *gUnk_02000128;
void SceneActorControl_ResetSingleton(void)
{
    gUnk_02000128 = 0;
}
