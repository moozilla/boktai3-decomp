#include "global.h"
#include "ram.h"

struct S {
    u8 f[0x19];
    u8 v;
};

u8 Wireless_GetConnectedSlotMask(void)
{
    if (((struct S *)gWirelessController) == 0)
        return 0;
    return ((struct S *)gWirelessController)->v;
}
