#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "scene/actor/scene_actors.h"

s32 Ratatosk_GetSelectedPointIndex(void)
{
    return ((struct Ratatosk *)SceneActors_GetBySlot(4))->selectedPoint;
}
