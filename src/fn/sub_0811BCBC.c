#include "global.h"
void Actor_ClampMovementSpeed(u8 *s)
{
    s32 *p = (s32 *)(s + 0x5dc);
    if (*p > 0xc8000)
        *p = 0xc8000;
    if (*p <= 0x1ffff)
        *p = 0x20000;
}
