#include "global.h"

struct S {
    u8 f[0x19];
    u8 v;
};
extern struct S *gWirelessController;

u8 Wireless_GetConnectedSlotMask(void)
{
    if (gWirelessController == 0)
        return 0;
    return gWirelessController->v;
}
