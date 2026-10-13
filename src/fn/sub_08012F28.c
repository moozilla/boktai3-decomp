#include "breakable_barrier.h"

static inline void disable(struct SceneCollider *c)
{
    c->state |= 4;
}
int sub_08012D20(struct BreakableBarrier *);
void BreakableBarrier_Hide(void *m, struct BreakableBarrier *p)
{
    if (sub_08012D20(p)) {
        p->sprite.flags |= 1;
        disable(&p->collider);
    }
}
