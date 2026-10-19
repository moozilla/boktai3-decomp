#include "global.h"
#include "area_effect_link.h"
#ifdef NONMATCHING
// fuzzy: 83.3%
// foundry draft (worker best 83.3%); what differs is mostly register allocation and instruction order
#define SessionMenu_InvokeCallback sub_0810BB0C
s32 Callback_InvokeActor(void *, void (*)(void));
static inline void SessionMenu_Call(struct AreaEffectLinkState *p)
{
    Callback_InvokeActor(p, p->menuCallback);
}
void sub_0810BB0C(void)
{
    struct AreaEffectLinkState *p = gUnk_020004B4;
    if (p) {
        struct AreaEffectLinkState *q = p;
        switch (q->unavailable) {
            case 0:
                SessionMenu_Call(q);
                break;
            case 1:
                Callback_InvokeActor(p, p->menuCallback);
                break;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0810BB0C);
#endif
