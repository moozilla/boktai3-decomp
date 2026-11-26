#include "monsters/ivy/phase_object.h"
u8 Callback_InvokeActor(void *, void *);
void Sound_PlaySong(u32);
void PhaseObject_UpdateEntryScatter(struct PhaseObject *);

/* The far-approach twin of ColorPulse_BeginNearApproach. */
void sub_080F2F24(struct PhaseObject *p)
{
    if (p->phaseUpdate && Callback_InvokeActor(p, p->phaseUpdate))
        p->actionCompletions++;
    if (p->actionCompletions > 1) {
        p->actionCompletions = 0;
        p->scatterTransition = PhaseObject_UpdateEntryScatter;
        Sound_PlaySong(0x16B);
    }
}
