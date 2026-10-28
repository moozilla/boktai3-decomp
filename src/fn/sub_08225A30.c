#include "global.h"
#include "scene_body_registry.h"
void SceneBody_DestroyRegistryTask(void)
{
    gSceneBodyRegistry = 0;
}
