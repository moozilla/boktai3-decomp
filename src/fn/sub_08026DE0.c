#include "scene_actors.h"
#include "scene_graphics_actor.h"

static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
static inline void decay(s16 *p)
{
    if (*p > 23)
        *p -= 24;
    else
        *p = 0;
}
void SceneActorGraphics_SelectResource(void *);
void SceneActors_UpdateShrinkingReaction(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct StateActor *p2 = p;
    if (p->actionChanged) {
        int sequence;
        int angle;
        p->actionChanged = 0;
        angle = p->bodyDirection;
        SceneActors_SetFacing(p, angle);
        sequence = choose(actor->graphics.animationFlags, 49, 11);
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 1, 4);
        SceneActorGraphics_SelectResource(p);
    }
    SceneGraphicsActor_ClearPendingDamage(actor);
    {
        u32 value;
        u32 mask = 12;
        value = p2->actionFlags | mask;
        value |= 64;
        p2->actionFlags = value;
    }
    if (actor->graphics.scaleMin > 23)
        actor->graphics.scaleMin -= 24;
    else
        actor->graphics.scaleMin = 0;
    decay(&actor->graphics.scaleMax);
}
