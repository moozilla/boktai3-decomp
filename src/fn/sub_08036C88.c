#include "global.h"

extern u8 *gWirelessController;

u16 Wireless_GetLocalSessionId(void)
{
    u16 r;
    if (gWirelessController)
        r = *(u16 *)(gWirelessController + 0x6C);
    else
        r = 0;
    return r;
}
