#include "actors/phase_object/phase_object.h"

void ChildGroup_UpdateTrailingMembers(struct PhaseObject *);
void PhaseObject_ApproachTarget(struct PhaseObject *);
void PhaseObject_EndApproachPause(struct PhaseObject *p)
{
    s32 i;
    if (!(p->frame & 3)) {
        if (--p->countdown == 0) {
            p->turnSpeed = -p->turnSpeed;
            PhaseObject_EnterMode(p, 1, 2);
            p->update = ChildGroup_UpdateTrailingMembers;
            i = 0;
            for (; i <= 3; i++) {
                p->entries[i].flags |= 1;
                p->memberCollidersA[i].flags |= 4;
                p->memberCollidersB[i].flags |= 4;
            }
            p->secondaryTransition = PhaseObject_ApproachTarget;
        }
    }
}
