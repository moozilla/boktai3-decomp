#define SceneActor_RampColorRedHigh sub_080F1CC4
#include "global.h"
#include "scene_actor_color_cycle.h"
s32 Div(s32, s32);
s32 Ivy_UpdateRedBrightening(struct SceneActorColorCycle *);
s32 sub_080F1CC4(struct SceneActorColorCycle *actor)
{
    actor->red = Div(actor->timer << 3, actor->duration) + 0x10;
    if (actor->timer++ == actor->duration) {
        actor->update = Ivy_UpdateRedBrightening;
        actor->timer = 0;
    }
    actor->color = (actor->blue << 10) | (actor->green << 5) | actor->red;
    return 0;
}
