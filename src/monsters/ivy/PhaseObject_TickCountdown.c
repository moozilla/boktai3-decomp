#include "monsters/ivy/phase_object.h"

void ChildGroup_UpdateTrailingMembers(struct PhaseObject *);
void sub_080F2BE4(struct PhaseObject *);
void PhaseObject_TickCountdown(struct PhaseObject *p)
{
    s32 i;

    if (!(p->frame & 3)) {
        if (p->countdown <= 0) {
            void (*next)(struct PhaseObject *);

            p->turnSpeed = -p->turnSpeed;
            PhaseObject_EnterMode(p, 1, 2);
            p->update = ChildGroup_UpdateTrailingMembers;
            for (i = 0; i < 4; i++) {
                p->entries[i].flags |= 1;
                /* assigned inside the loop: the ROM loads it between the loop invariants */
                next = sub_080F2BE4;
                p->memberCollidersA[i].flags |= 4;
                p->memberCollidersB[i].flags |= 4;
            }
            p->secondaryTransition = next;
        } else {
            s32 remaining;

            p->countdown -= p->countdownStep;
            remaining = p->countdown;
            if (remaining < 0)
                remaining = 0;
            p->countdown = remaining;
        }
    }
}
