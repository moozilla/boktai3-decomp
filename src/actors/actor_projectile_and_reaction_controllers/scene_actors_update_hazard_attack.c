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
void AnimatedHazard_Create(void *, int, int);
void SceneActors_UpdateHazardAttack(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct StateActor *p2 = p;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        sequence = choose(actor->graphics.animationFlags, 64, 26);
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 1, 4);
        Sound_PlaySongChecked(856);
    }
    if (!p->actionTryUpdate(p) && !p->actionReset(p)) {
        if (tick == 16) {
            AnimatedHazard_Create(&p->targetPosition, 1, 60);
            Sound_PlaySongChecked(849);
        }
        if (((struct SceneGraphicsActor *)p2)->animation.status & 2)
            p->actionFinished = 1;
    }
}
