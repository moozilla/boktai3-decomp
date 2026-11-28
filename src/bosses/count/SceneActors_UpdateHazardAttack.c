#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

void AnimatedHazard_Create(void *, int, int);
void SceneActors_UpdateHazardAttack(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 26;
        else
            sequence = 64;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 1, 4);
        Sound_PlaySongChecked(856);
    }
    if (!p->actionTryUpdate(p) && !p->actionReset(p)) {
        if (tick == 16) {
            AnimatedHazard_Create(&p->targetPosition, 1, 60);
            Sound_PlaySongChecked(849);
        }
        if (actor->animation.status & 2)
            p->actionFinished = 1;
    }
}
