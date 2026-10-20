// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "area_effect_link.h"
s32 Callback_InvokeActor(void *, void (*)(void));
void SessionMenu_InvokeCallback(void)
{
    register struct AreaEffectLinkState *p asm("r3") = gUnk_020004B4;
    if (p) {
        register struct AreaEffectLinkState *q asm("r2") = p;
        switch (q->unavailable) {
            case 0:
                Callback_InvokeActor(q, q->menuCallback);
                break;
            case 1:
                Callback_InvokeActor(p, p->menuCallback);
                break;
        }
    }
}
