#include "global.h"
#include "ram.h"


u32 Wireless_IsSessionActive(void)
{
    return ((u8 *)gWirelessController) ? ((u8 *)gWirelessController)[0x30] : 0x0;
}
