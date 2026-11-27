#include "global.h"
#include "scene/collision/scene_body_registry.h"
void SceneBody_DestroyRegistryTask(void)
{
    gSceneBodyRegistry = 0;
}
