#include "global.h"
struct ColorPulse {
    u8 pad0[0xB4C];
    u16 periodB4C;
    u16 timerB4E;
    u8 padB50[0xCE4 - 0xB50];
    void (*updateCE4)(struct ColorPulse *);
    u8 padCE8[0xCF0 - 0xCE8];
    void (*callbackCF0)(struct ColorPulse *);
    u8 padCF4[0xD53 - 0xCF4];
    s8 approaches;
};
u8 Callback_InvokeActor(void *, void *);
void ChildGroup_LaunchNextInactiveMember(struct ColorPulse *);

/* Twin of ColorPulse_BeginNearApproach: after two approaches, restart the
   timer and hand over to ChildGroup_LaunchNextInactiveMember. */
void sub_080F33BC(struct ColorPulse *p)
{
    if (p->callbackCF0 && Callback_InvokeActor(p, p->callbackCF0))
        p->approaches++;
    if (p->approaches > 1) {
        p->approaches = 0;
        p->timerB4E = p->periodB4C;
        p->updateCE4 = ChildGroup_LaunchNextInactiveMember;
    }
}
