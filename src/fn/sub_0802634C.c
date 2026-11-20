#include "scene_actors.h"
#include "scene_graphics_actor.h"

void SceneActorAnimation_ResetActionState(struct StateActor *p)
{
    if (p->actionChanged)
        p->actionChanged = 0;
    SceneGraphicsActor_ClearPendingDamage((struct SceneGraphicsActor *)p);
    {
        u32 value;
        u32 mask = 12;
        value = p->actionFlags | mask;
        value |= 0x10010;
        p->actionFlags = value;
    }
    p->actionReset(p);
}
