#include "global.h"

extern s32 gUnk_030053F4;
void PlayerAction_RefreshModifiers(u8 *);

void Player_OnMotion10Exit(u8 *p)
{
    if (!(gUnk_030053F4 & 0x1000)) {
        PlayerAction_RefreshModifiers(p);
    }
    *(u8 *)(p + 0xAC8) = 0xFF;
}
