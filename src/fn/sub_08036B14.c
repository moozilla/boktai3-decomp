#include "global.h"
#include "ram.h"
void Wireless_RequestAdvanceConnection(void)
{
    u8 *p = gWirelessController;
    if (p)
        p[0x1C] = 1;
}
