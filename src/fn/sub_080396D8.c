#include "global.h"
u8 *WirelessExchange_Get(void);
void WirelessGame_NotifyPlayerDefeated(void)
{
    u8 *p = WirelessExchange_Get();
    if (p)
        p[0x2D] = 1;
}
