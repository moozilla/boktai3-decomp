#include "global.h"
#include "ram.h"


u16 Wireless_GetLocalSessionId(void)
{
    u16 r;
    if (((u8 *)gWirelessController))
        r = *(u16 *)(((u8 *)gWirelessController) + 0x6C);
    else
        r = 0;
    return r;
}
