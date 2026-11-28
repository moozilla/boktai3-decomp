#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "scene/actor/scene_actors.h"

s32 Ratatosk_GetSelectedTargetIndex(void)
{
    return ((struct Ratatosk *)SceneActors_GetBySlot(4))->selectedTarget;
}
