#define Player_UpdateResetBlink sub_08243678
#define Player_AddResetFlags sub_08238450
#include "global.h"
#include "player.h"
void sub_08238450(struct Player *, u32);

/* Tentative: toggles presentation flags while counting down a reset value. */
void sub_08243678(struct Player *p)
{
    s32 timer = p->positionResetValue;
    if (timer > 0) {
        sub_08238450(p, 8);
        if (p->directionFrame == 0 && p->tetherMode != 0x1D && p->tetherMode != 0x1E) {
            timer--;
            if (timer != 0 && ((timer >> 2) & 1))
                sub_08238450(p, 5);
            p->positionResetValue = timer;
        }
    }
}
