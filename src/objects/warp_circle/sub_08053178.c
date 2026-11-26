#include "objects/warp_circle/warp_circle.h"
void WarpCircle_ConsumeTriggers(struct WarpCircle *);
void Callback_InvokeActor(struct WarpCircle *, void (*)(struct WarpCircle *));
void WarpCircle_TickParticle(struct WarpCircleParticle *);
s32 WarpCircle_Tick(struct WarpCircle *actor)
{
    s32 i;
    WarpCircle_ConsumeTriggers(actor);
    Callback_InvokeActor(actor, actor->update);
    for (i = 0; i < 8; i++)
        if (actor->particles[i].enabled)
            WarpCircle_TickParticle(&actor->particles[i]);
    return 0;
}
