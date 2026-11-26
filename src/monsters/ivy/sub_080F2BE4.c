#include "monsters/ivy/phase_object.h"
void PhaseObject_TickCountdown(struct PhaseObject *);

/* Every fourth frame, grow value by step until it reaches limit, then
   hand over to PhaseObject_TickCountdown. */
void sub_080F2BE4(struct PhaseObject *p)
{
    if (!(p->frame & 3)) {
        if (p->countdown >= p->countdownLimit)
            p->secondaryTransition = PhaseObject_TickCountdown;
        else
            p->countdown += p->countdownStep;
    }
}
