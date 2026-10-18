#include "global.h"

void Bee_TickActionCooldown(u8 *p)
{
    u8 *q = p + 0x42b;
    u8 v = *q;
    if (v != 0)
        *q = v - 1;
}
