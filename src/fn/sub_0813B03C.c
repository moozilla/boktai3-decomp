#include "global.h"
void Player_SetActionPhase(u8 *p, u8 v)
{
    u8 *a = p + 0x458;
    u16 z = 0;
    *a = v;
    *(u16 *)(p + 0x45a) = z;
}
