#include "monsters/ivy/phase_object.h"
u32 Callback_InvokeActor(void *, void *);
void sub_080F2BE4(struct PhaseObject *);

void Ivy_WaitForSecondaryActionCompletion(struct PhaseObject *p)
{
    if (p->phaseUpdate && (u8)Callback_InvokeActor(p, p->phaseUpdate))
        p->actionCompletions++;
    if (p->actionCompletions > 1) {
        p->actionCompletions = 0;
        p->secondaryTransition = (void *)sub_080F2BE4;
    }
}
