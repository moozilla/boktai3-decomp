#include "global.h"

extern s32 gUnk_030053F4;
void sub_08159594(u8 *);

void Player_OnMotion10Exit(u8 *p)
{
    if (!(gUnk_030053F4 & 0x1000)) {
        sub_08159594(p);
    }
    *(u8 *)(p + 0xAC8) = 0xFF;
}
