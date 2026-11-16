#include "global.h"

void SceneActorControl_UpdateHoverHeight(struct ChaseActor *actor);
void Axe_UpdateRouteWander(struct ChaseActor *actor);
void sub_080FB30C(struct ChaseActor *actor);

s32 Axe_PrepareUpdate(void *p)
{
    SceneActorControl_UpdateHoverHeight(p);
    Axe_UpdateRouteWander(p);
    sub_080FB30C(p);
    return 1;
}
