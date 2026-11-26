#include "bosses/hresvelgr/hresvelgr_travel.h"

void Hresvelgr_SetAnimation(struct HresvelgrTravelActor *, void *, void *, s32, s32, s32);
s32 Callback_InvokeActor(void *, void *);

void sub_0819E7B0(struct HresvelgrTravelActor *p)
{
    if (p->changed != 0) {
        p->changed = 0;
        Hresvelgr_SetAnimation(p, &p->animation, &p->animationData, 0, 1, 0);
    }
    Callback_InvokeActor(p, p->callback);
}
