#include "monsters/ivy/phase_object.h"

s32 Div(s32, s32);

void ChildGroup_UpdateAlternatingCallbacks(struct PhaseObject *);
void PhaseObject_UpdateEntryScatter(struct PhaseObject *);
// confidence=low: interpolates the four entries toward the shared target before enabling them.
void PhaseObject_ConvergeEntries(struct PhaseObject *p)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        p->entries[i].presentation.x =
            p->goal.x + Div((p->goal.x - p->saved[i].x) * p->scatterDelay, 15);
        p->entries[i].presentation.z =
            p->goal.z + Div((p->goal.z - p->saved[i].z) * p->scatterDelay, 15);
        p->entries[i].presentation.opacity += 3;
        if (p->entries[i].presentation.opacity > 63)
            p->entries[i].presentation.opacity = 64;
    }
    if (!(p->frame & 1) && --p->scatterDelay == 0) {
        for (i = 0; i < 4; i++) {
            p->entries[i].presentation.x = p->goal.x;
            p->entries[i].presentation.z = p->goal.z;
            p->entries[i].presentation.opacity = 64;
            p->entries[i].flags |= 1;
        }
        p->drawableFlags &= ~1;
        p->colliderA.flags &= ~4;
        p->colliderB.flags &= ~4;
        PhaseObject_EnterMode(p, 1, 5);
        p->update = ChildGroup_UpdateAlternatingCallbacks;
        p->scatterTransition = PhaseObject_UpdateEntryScatter;
    }
}
