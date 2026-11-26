#include "actors/common_scene_actor_controller/scene_actors.h"
#include "engine/shared/scene_graphics_actor.h"

static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
void SceneActors_UpdateUnmirroredReaction(struct StateActor *p)
{
    struct StateActor *p2 = p;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        sequence = choose(((struct SceneGraphicsActor *)p)->graphics.animationFlags, 70, 32);
        SceneActors_StartUnmirroredAnimation(
            p, &((struct SceneGraphicsActor *)p)->animation,
            (int)&((struct SceneGraphicsActor *)p)->animationResource, sequence, 1, 4);
    }
    SceneGraphicsActor_ClearPendingDamage((struct SceneGraphicsActor *)p);
    SceneActorState_SetFlags(p2, 12);
    if (!p2->actionReset(p2))
        ((struct SceneGraphicsActor *)p)->graphics.reactionDelay = 8;
}
