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
static inline void effect(u8 *p, int mode, int id, int duration)
{
    *(u32 *)(p + 0x390) = id;
    p[0x38e] = mode;
    p[0x38f] = duration;
    if (p[0x38e] == 2) {
        u32 value = duration == 5 ? 32 : 64;
        p[0x38c] = value;
    } else if (p[0x38e] == 1)
        p[0x38c] = 0;
}
void SceneActorAnimation_UpdateImpactRelease(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        sequence = choose(actor->graphics.animationFlags, 62, 24);
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 1, 1, 6);
        actor->graphics.scaleMin = 0;
        actor->graphics.scaleMax = 0;
        actor->variant = 0;
        effect((u8 *)p, 2, 827, 6);
    }
    SceneGraphicsActor_ClearPendingDamage(actor);
    {
        u32 mask = 12;
        p->actionFlags |= mask;
    }
    if (!p->actionReset(p) && (actor->animation.status & 2))
        p->actionFinished = 1;
}
