#include "monsters/ivy/phase_object.h"
u8 Callback_InvokeActor(void *, void *);
void Sound_PlaySong(u32);
void Ivy_UpdatePositionTransition(struct PhaseObject *);

void ColorPulse_BeginNearApproach(struct PhaseObject *p)
{
    if (p->phaseUpdate && Callback_InvokeActor(p, p->phaseUpdate))
        p->actionCompletions++;
    if (p->actionCompletions > 1) {
        p->actionCompletions = 0;
        p->turnFrames = 0;
        p->transition = Ivy_UpdatePositionTransition;
        Sound_PlaySong(0x18F);
    }
}
