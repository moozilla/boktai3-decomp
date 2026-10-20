// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"

void Player_AddControlFlags(struct Player *, u32);

void Player_UpdateResetBlink(struct Player *p)
{
    register s32 value asm("r5");
    value = p->positionResetValue;
    if (value > 0) {
        Player_AddControlFlags(p, 8);
        if (p->directionFrame == 0) {
            u32 mode = p->tetherMode;
            if (mode == 0x1D)
                goto done;
            if (mode != 0x1E) {
                value--;
                if (value != 0 && ((value >> 2) & 1) != 0)
                    Player_AddControlFlags(p, 5);
                p->positionResetValue = value;
            }
        }
    }
done:;
}
