#include "global.h"
#include "ram.h"

void SceneActors_Register(u32 v, u32 i)
{
    gSceneActorRegistry[i] = v;
}
