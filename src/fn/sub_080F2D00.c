#include "global.h"
int Callback_InvokeActor(void *, void *);
void PhaseObject_ApproachTarget(void *);
struct S {
    u8 pad0[0xcdc];
    void (*update)(void *);
    u8 padCE0[0xcf0 - 0xce0];
    void *callback;
    u8 padCF4[0xd53 - 0xcf4];
    s8 hits;
};

void sub_080F2D00(struct S *p)
{
    if (p->callback && (u8)Callback_InvokeActor(p, p->callback))
        p->hits++;
    if (p->hits > 2) {
        p->hits = 0;
        p->update = PhaseObject_ApproachTarget;
    }
}
