#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
void SceneActors_UpdateAnchoredReaction(struct StateActor *p)
{
    struct StateActor *p2 = p;
    u8 *position = (u8 *)p + 0x48;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        sequence = choose(((struct SceneGraphicsActor *)p)->graphics.animationFlags, 49, 11);
        SceneActors_StartDirectionalAnimation(
            p, &((struct SceneGraphicsActor *)p)->animation,
            (int)&((struct SceneGraphicsActor *)p)->animationResource, sequence, 2, 1, 4);
        p->targetPosition = *(struct StateActorPos *)(position + 12);
    }
    SceneGraphicsActor_ClearPendingDamage((struct SceneGraphicsActor *)p);
    SceneActorState_SetFlags(p2, 12);
    if (!p2->actionReset(p2))
        ((struct SceneGraphicsActor *)p)->graphics.reactionDelay = 6;
}
