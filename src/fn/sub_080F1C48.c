#include "global.h"
#include "scene_actor_color_cycle.h"
s32 Div(s32, s32);
s32 SceneActor_RampColorRedHigh(struct SceneActorColorCycle *);
s32 SceneActor_RampColorRed(struct SceneActorColorCycle *actor)
{
    actor->red = Div(actor->timer << 4, actor->duration);
    if (actor->timer++ == actor->duration) {
        actor->update = SceneActor_RampColorRedHigh;
        actor->timer = 0;
    }
    actor->color = (actor->blue << 10) | (actor->green << 5) | actor->red;
    return 0;
}
