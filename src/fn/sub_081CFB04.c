#include "global.h"

// confidence=low: an Actor_OnIdleUpdate variant: plays animation 14 on entry, runs the actor's
// callback, and finishes when it does not return 1 and the animation has ended.
struct Actor {
    u8 pad0[7];
    u8 done;
    u8 pad8[1];
    u8 entered;
    u8 padA[0xd4 - 0xa];
    u8 display[0x2e4 - 0xd4];
    void *callback;
};
int Actor_PlayAnimationEntry(struct Actor *, int, int, int, int);
u8 Callback_InvokeActor(void *, void *);
s32 DisplayAnimation_TestFinished(void *);
void sub_081CFB04(void *unused, struct Actor *p)
{
    if (p->entered) {
        int zero = 0;
        p->entered = zero;
        p->done = zero;
        Actor_PlayAnimationEntry(p, 14, 2, zero, 4);
    }
    if (Callback_InvokeActor(p, p->callback) != 1 && DisplayAnimation_TestFinished(p->display))
        p->done = 1;
}
