#include "global.h"
void Player_SetTetherMode(void *, u32, u32);
void Callback_InvokeActor(void *, u32);
void Player_RunConfiguredActionHandler(u8 *s)
{
    if (*(u8 *)(s + 0x457) != 3)
        Player_SetTetherMode(s, 3, 0);
    Callback_InvokeActor(s, *(u32 *)(s + 0xc88));
}
