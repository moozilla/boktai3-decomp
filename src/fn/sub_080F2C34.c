#include "phase_object.h"

void ChildGroup_UpdateTrailingMembers(struct PhaseObject *);
void sub_080F2BE4(struct PhaseObject *);
void PhaseObject_TickCountdown(struct PhaseObject *p)
{
    s32 i;
    if (!(p->frame & 3)) {
        if (p->countdown <= 0) {
            void (*callback)(struct PhaseObject *);
            p->turnSpeed = -p->turnSpeed;
            PhaseObject_EnterMode(p, 1, 2);
            p->update = ChildGroup_UpdateTrailingMembers;
            for (i = 0; i <= 3; i++) {
                p->entries[i].flags |= 1;
                /* set in the loop: loop.c hoists it between the loop's
                   other invariants, where the ROM loads it */
                callback = sub_080F2BE4;
                p->memberCollidersA[i].flags |= 4;
                p->memberCollidersB[i].flags |= 4;
            }
            p->secondaryTransition = callback;
        } else {
            p->countdown -= p->countdownStep;
            {
                s32 n = p->countdown;
                if (n < 0)
                    n = 0;
                p->countdown = n;
            }
        }
    }
}
