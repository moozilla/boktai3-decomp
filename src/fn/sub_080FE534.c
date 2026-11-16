#include "scene_actor_state.h"
extern void *gUnk_08606494[];

void Axe_InitEventCallbacks(struct ChaseActor *p)
{
    p->base.alternateBehaviorCallbacks = gUnk_08606494;
}
