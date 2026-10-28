#include "global.h"
#include "ram.h"

u32 SceneActors_GetBySlot(u32 i)
{
    return gSceneActorRegistry[i];
}
