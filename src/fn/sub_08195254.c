#include "global.h"
#include "game_state.h"

extern const s16 gUnk_0824F598[];

void sub_08195254(u8 *t)
{
    s32 d = gUnk_02000710->positionZ - (*(s16 *)(t + 0x5410) - gUnk_0824F598[2]);
    s32 target;
    s16 *speed;
    if (d > 0x200)
        target = 40;
    else if (d > 0x40)
        target = 12;
    else if (d > 0)
        target = 4;
    else if (d == 0)
        target = 0;
    else if (d > -0x40)
        target = -4;
    else if (d > -0x200)
        target = -12;
    else
        target = -40;
    speed = (s16 *)(t + 0x5414);
    d = target - *speed;
    if (d > 16)
        *speed += 4;
    else if (d > 4)
        *speed += 2;
    else if (d > 0)
        *speed += 1;
    else if (d != 0) {
        if (d > -4)
            *speed -= 1;
        else if (d > -16)
            *speed -= 2;
        else
            *speed -= 4;
    }
    *(s16 *)(t + 0x5410) += *(s16 *)(t + 0x5414);
}
