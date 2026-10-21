#include "global.h"

void SceneActorControl_UpdateHoverHeight(void *);
void Axe_UpdateRouteWander(void *);
void sub_080FB30C(void *);

s32 Axe_PrepareUpdate(void *p)
{
    SceneActorControl_UpdateHoverHeight(p);
    Axe_UpdateRouteWander(p);
    sub_080FB30C(p);
    return 1;
}
