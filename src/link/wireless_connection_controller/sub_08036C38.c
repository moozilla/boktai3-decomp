#include "engine/shared/global.h"
#include "engine/shared/ram.h"


void Wireless_RequestResetConnection(void)
{
    if (((u8 *)gWirelessController)) {
        ((u8 *)gWirelessController)[0x3D] = 1;
    }
}
