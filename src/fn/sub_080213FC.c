#include "global.h"
#include "ram.h"

void SceneActors_Unregister(u32 i)
{
    gSceneActorRegistry[i] = 0;
}
