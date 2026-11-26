#include "engine/shared/global.h"
#include "engine/shared/ram.h"
void Wireless_RequestAdvanceConnection(void)
{
    u8 *p = gWirelessController;
    if (p)
        p[0x1C] = 1;
}
