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
static inline void effect(u8 *p, int id, int mode, int duration)
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
void SceneActorAnimation_UpdateImpactFinish(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (((struct StateActor *)actor)->actionChanged) {
        int sequence;
        ((struct StateActor *)actor)->actionChanged = 0;
        sequence = choose(actor->graphics.animationFlags, 62, 24);
        SceneActors_StartDirectionalAnimation((struct StateActor *)actor, &actor->animation,
                                              (int)&actor->animationResource, sequence, 1, 1, 4);
        effect((u8 *)actor, 827, 1, 5);
        Sound_PlaySongChecked(838);
        Sound_PlaySongChecked(1334);
    }
    SceneGraphicsActor_ClearPendingDamage(actor);
    SceneActorState_SetFlags(p, 12);
    {
        u8 result = p->actionReset(p);
        if (!result && tick > 31) {
            p->actionFinished = 1;
            actor->graphics.scaleMin = result;
            actor->graphics.scaleMax = result;
            actor->variant = 0;
        }
    }
}
