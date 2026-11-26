#include "engine/shared/global.h"
#include "engine/scene_body/scene_body_registry.h"
u32 *SceneBody_GetRegistry(void)
{
    if (gSceneBodyRegistry == 0)
        return 0;
    return (u32 *)gSceneBodyRegistry;
}
