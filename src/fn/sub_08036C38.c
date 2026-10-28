#include "global.h"

extern u8 *gWirelessController;

void Wireless_RequestResetConnection(void)
{
    if (gWirelessController) {
        gWirelessController[0x3D] = 1;
    }
}
