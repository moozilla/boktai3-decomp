#include "global.h"
void Player_InitializeActionState(void *, u32);
void Player_InitConfiguredAction(u8 *s)
{
    Player_InitializeActionState(s, *(u32 *)(s + 0x344) + 0x1c);
}
