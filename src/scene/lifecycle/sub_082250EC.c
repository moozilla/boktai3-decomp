#include "global.h"

void SceneActorRegistry_ClearObjectContext(void);
void Camera_ClearInstance(void);

void SceneLifecycle_ResetSceneRegistries(void)
{
    SceneActorRegistry_ClearObjectContext();
    Camera_ClearInstance();
}
