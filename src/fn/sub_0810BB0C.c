// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "area_effect_link.h"
s32 Callback_InvokeActor(void *, void (*)(void));
void SessionMenu_InvokeCallback(void)
{
    struct AreaEffectLinkState *p = gLinkSession;
    DEBUG_NOP();
    if (p) {
        struct AreaEffectLinkState *q = gLinkSession;
        switch (q->unavailable) {
            case 0:
                Callback_InvokeActor(q, q->menuCallback);
                break;
            case 1:
                Callback_InvokeActor(gLinkSession, gLinkSession->menuCallback);
                break;
        }
    }
}
