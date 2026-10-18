#define SceneActor_RampColorRed     sub_080F1C48
#define SceneActor_RampColorRedHigh sub_080F1CC4
#include "global.h"
#include "scene_actor_color_cycle.h"
s32 Div(s32, s32);
s32 sub_080F1CC4(struct SceneActorColorCycle *);
s32 sub_080F1C48(struct SceneActorColorCycle *actor)
{
    actor->red = Div(actor->timer << 4, actor->duration);
    if (actor->timer++ == actor->duration) {
        actor->update = sub_080F1CC4;
        actor->timer = 0;
    }
    actor->color = (actor->blue << 10) | (actor->green << 5) | actor->red;
    return 0;
}
