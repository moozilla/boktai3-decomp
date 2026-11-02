#include "global.h"
#include "area_effect_link.h"
s32 Callback_InvokeActor(void *, void (*)(void));
static inline struct AreaEffectLinkState *
SessionMenu_GetCurrent(struct AreaEffectLinkState *const *slot)
{
    return *slot;
}

void SessionMenu_InvokeCallback(void)
{
    struct AreaEffectLinkState *session = gLinkSession;

    if (gLinkSession) {
        struct AreaEffectLinkState *selected = SessionMenu_GetCurrent(&session);
        switch (gLinkSession->unavailable) {
            case 0:
                Callback_InvokeActor(selected, selected->menuCallback);
                break;
            case 1:
                Callback_InvokeActor(session, session->menuCallback);
                break;
        }
    }
}
