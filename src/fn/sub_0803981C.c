#include "global.h"
u8 *WirelessExchange_Get(void);
void WirelessGame_ResetCyclingItemLatch(void)
{
    u8 *p = WirelessExchange_Get();
    if (p)
        p[0xEB] = 0;
}
