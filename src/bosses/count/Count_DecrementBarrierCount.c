#include "global.h"
#include "bosses/count/scene_graphics_actor.h"

struct SceneGraphicsActor *SceneActors_GetBySlot(u32);

void Count_DecrementBarrierCount(void)
{
    struct SceneGraphicsActor *actor = SceneActors_GetBySlot(0);
    if (actor && actor->graphics.barrierCount)
        actor->graphics.barrierCount--;
}
