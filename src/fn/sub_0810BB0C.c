#include "global.h"
#include "area_effect_link.h"
s32 Callback_InvokeActor(void *, void (*)(void));
void SessionMenu_InvokeCallback(void)
{
    struct AreaEffectLinkState *p = gUnk_020004B4;
    DEBUG_NOP();
    if (p) {
        struct AreaEffectLinkState *q = gUnk_020004B4;
        switch (q->unavailable) {
            case 0:
                Callback_InvokeActor(q, q->menuCallback);
                break;
            case 1:
                Callback_InvokeActor(gUnk_020004B4, gUnk_020004B4->menuCallback);
                break;
        }
    }
}
