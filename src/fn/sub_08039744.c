#include "global.h"
u8 *WirelessExchange_Get(void);
static inline u8 g(u8 *q)
{
    return q[5];
}
u32 WirelessGame_GetRemoteOpponentHealthPercent(void)
{
    u8 *p = WirelessExchange_Get();
    u32 r;
    if (p)
        r = g(p + 0x48);
    else
        r = 0;
    return r;
}
