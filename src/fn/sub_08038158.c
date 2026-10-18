#include "global.h"
u8 *WirelessExchange_Get(void);
void WirelessGameMenu_OnResultPanelClosed(void)
{
    u8 *p = WirelessExchange_Get();
    if (p)
        p[0x34] = 1;
}
