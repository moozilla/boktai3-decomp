#include "global.h"
#include "player.h"

void Player_AddControlFlags(struct Player *, u32);

void Player_UpdateResetBlink(struct Player *p)
{
    s32 value;
    struct Player *q;
    value = p->positionResetValue;
    if (value > 0) {
        Player_AddControlFlags(p, 8);
        if (p->directionFrame == 0) {
            u32 mode = p->tetherMode;
            if (mode == 0x1D)
                goto done;
            if (mode != 0x1E) {
                q = p;
                value--;
                if (value != 0 && ((value >> 2) & 1) != 0)
                    Player_AddControlFlags(q, 5);
                q->positionResetValue = value;
            }
        }
    }
done:;
}
