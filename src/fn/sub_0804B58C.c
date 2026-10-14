#include "tether_actor.h"

void TetherActor_SelectInteractionState(struct TetherActor *);
void TetherActor_UpdateInteractionValue(struct TetherActor *p)
{
    if (!(gUnk_030053F4 & 0x200)) {
        if (p->interactionEnabled != 0) {
            if (p->interactionReset == 0) {
                if (p->statusFlags == 0) {
                    u16 *q = &p->currentValue;
                    if (*q != 0)
                        *q = *q - 1;
                    else
                        TetherActor_SelectInteractionState(p);
                }
            }
        }
    }
}
