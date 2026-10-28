#include "global.h"
#include "ram.h"


void Wireless_RequestResetConnection(void)
{
    if (((u8 *)gWirelessController)) {
        ((u8 *)gWirelessController)[0x3D] = 1;
    }
}
