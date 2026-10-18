#include "global.h"
void Registry1745_ClearParticipantCounters(u8 *p)
{
    u16 *h;
    s32 n;
    u32 z;
    *(u16 *)(p + 0x7a) = 0;
    z = 0;
    n = 3;
    h = (u16 *)(p + 0x82);
    do {
        *h = z;
        h--;
        n--;
    } while (n >= 0);
}
