#include "npc/otenko/otenko_actor.h"
u32 Otenko_OnStateUpdate(void *a, struct OtenkoActor *p)
{
    u32 n;
    void (*fn)(void *, void *, u32);
    p->displayFlags = 0;
    p->done = 0;
    n = p->stateFrame++;
    fn = (void (*)(void *, void *, u32))p->stateCallback;
    if (fn)
        fn(a, p, n);
    return 0;
}
