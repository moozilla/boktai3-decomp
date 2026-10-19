#define Player_UpdateBlinkTimer sub_0815EBB8
#define Player_AddVisualFlags sub_0813B134
#include "global.h"
#include "player.h"
void sub_0813B134(struct Player *, s32);
void sub_0815EBB8(struct Player *arg)
{
    struct Player *p = arg;
    s32 timer;
    timer = p->positionResetValue;
    if (timer > 0) {
        sub_0813B134(p, 8);
        if (p->directionFrame == 0 && p->tetherMode != 0x1D && p->tetherMode != 0x1E) {
            timer = timer - 1;
            if (timer != 0 && ((timer >> 2) & 1))
                sub_0813B134(p, 5);
            p->positionResetValue = timer;
        }
    }
}
