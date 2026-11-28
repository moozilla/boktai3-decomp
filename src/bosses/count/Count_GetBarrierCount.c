#include "global.h"
#include "bosses/count/scene_graphics_actor.h"

struct SceneGraphicsActor *SceneActors_GetBySlot(u32);

u8 Count_GetBarrierCount(void)
{
    struct SceneGraphicsActor *actor = SceneActors_GetBySlot(0);
    u8 count;
    if (actor)
        count = actor->graphics.barrierCount;
    else
        count = 0;
    return count;
}
