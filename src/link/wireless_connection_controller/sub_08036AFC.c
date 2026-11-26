#include "engine/shared/global.h"
#include "engine/shared/ram.h"
void Wireless_RequestDisconnectPeer(void)
{
    u8 *p = gWirelessController;
    if (p)
        p[0x1b] = 1;
}
