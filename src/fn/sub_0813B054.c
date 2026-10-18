#include "global.h"
void Player_SetPendingCallback(u8 *p, u32 v)
{
    *(u32 *)(p + 0xca8) = v;
}
