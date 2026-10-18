#include "global.h"

void SceneActorControl_UpdateHoverHeight(void *);
void sub_080FAE50(void *);
void sub_080FB30C(void *);

s32 Axe_PrepareUpdate(void *p)
{
    SceneActorControl_UpdateHoverHeight(p);
    sub_080FAE50(p);
    sub_080FB30C(p);
    return 1;
}
