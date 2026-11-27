#include "npc/otenko/otenko_actor.h"

// confidence=low: an Actor_OnIdleUpdate variant: plays animation 14 on entry, runs the actor's
// callback, and finishes when it does not return 1 and the animation has ended.
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
void sub_081CFB04(void *unused, struct OtenkoActor *p)
{
    if (p->animationEntered) {
        p->animationEntered = 0;
        p->done = 0;
        Actor_PlayAnimationEntry(p, 14, 2, 0, 4);
    }
    if ((u8)Callback_InvokeActor(p, p->animationCallback) != 1 &&
        DisplayAnimation_TestFinished(&p->display))
        p->done = 1;
}
