#include "global.h"
#include "player.h"
void Player_SetSecondaryActionFlags(struct Player *, u32);
void Player_TickDamageBlink(struct Player *p)
{
    struct Player *q;
    s32 n;
    n = p->positionResetValue;
    if (n > 0) {
        Player_SetSecondaryActionFlags(p, 8);
        if (p->directionFrame == 0 && p->tetherMode != 0x1D && p->tetherMode != 0x1E) {
            q = p;
            n--;
            if (n != 0 && ((n >> 2) & 1) != 0)
                Player_SetSecondaryActionFlags(p, 5);
            q->positionResetValue = n;
        }
    }
}
