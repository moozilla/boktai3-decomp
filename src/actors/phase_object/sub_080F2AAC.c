#include "actors/phase_object/phase_object.h"

void Ivy_InitBobbing(struct PhaseObject *);
void ChildGroup_UpdateTimedOscillation(struct PhaseObject *);
void Ivy_WaitForPrimaryActionCompletion(struct PhaseObject *);
void PhaseObject_TickEntryReset(struct PhaseObject *p)
{
    s32 i;
    if (!(p->frame & 7)) {
        if (++p->turnSpeed > 7)
            p->turnSpeed = 7;
    }
    for (i = 0; i < p->turnSpeed; i++) {
        if (++p->rotation == 0x80) {
            for (; i <= 3; i++) {
                p->entries[i].phase = 0x80;
                p->entries[i].flags |= 1;
            }
            p->turnSpeed = 0;
            Ivy_InitBobbing(p);
            PhaseObject_EnterMode(p, 1, 0);
            p->update = ChildGroup_UpdateTimedOscillation;
            p->transition = Ivy_WaitForPrimaryActionCompletion;
        }
    }
}
