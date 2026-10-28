#include "global.h"

extern u8 *gWirelessController;

u32 Wireless_IsSessionActive(void)
{
    return gWirelessController ? gWirelessController[0x30] : 0x0;
}
