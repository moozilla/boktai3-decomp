#include "engine/shared/global.h"
#include "engine/scene_body/scene_body_registry.h"
void SceneBody_DestroyRegistryTask(void)
{
    gSceneBodyRegistry = 0;
}
