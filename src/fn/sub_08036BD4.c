#include "global.h"

extern u32 gWirelessController;

void Wireless_SetStatus(u32 v)
{
    if (gWirelessController != 0) {
        *(u8 *)(gWirelessController + 0x33) = v;
    }
}
