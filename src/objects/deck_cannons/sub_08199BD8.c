#include "global.h"
#include "objects/deck_cannons/callback_slot_layout.h"
// confidence=low: moves 20 units along x (flag 4) or y for three frames, then switches to state 3.
void sub_08199BD8(struct CallbackSlot *p)
{
    if (p->flags & 4)
        p->position.x += 20;
    else
        p->position.z += 20;
    p->timer++;
    if (p->timer > 2) {
        u8 state = 3;
        p->state = state;
        p->phase = 0;
        p->timer = 0;
    }
}
