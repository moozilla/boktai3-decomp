#include "monsters/ivy/phase_object.h"
u8 Callback_InvokeActor(void *, void *);
void PhaseObject_ApproachTarget(struct PhaseObject *);

void sub_080F2D00(struct PhaseObject *p)
{
    if (p->phaseUpdate && (u8)Callback_InvokeActor(p, p->phaseUpdate))
        p->actionCompletions++;
    if (p->actionCompletions > 2) {
        p->actionCompletions = 0;
        p->secondaryTransition = PhaseObject_ApproachTarget;
    }
}
