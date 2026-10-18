#define SceneActor_LowerColorRedPeak sub_080F1E00
#include "global.h"
#include "scene_actor_color_cycle.h"
s32 Div(s32, s32);
s32 ColorPulse_FadeRed(struct SceneActorColorCycle *);
s32 sub_080F1E00(struct SceneActorColorCycle *actor)
{
    actor->red = Div(actor->timer * 7, actor->duration) + 24;
    if (--actor->timer == -1) {
        actor->update = ColorPulse_FadeRed;
        actor->timer = actor->duration;
    }
    actor->color = (actor->blue << 10) | (actor->green << 5) | actor->red;
    return 0;
}
