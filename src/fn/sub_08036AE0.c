#include "global.h"

struct S {
    u8 pad0[56];
    u16 f2;
    u32 f4;
    u32 f8;
};
extern struct S *gWirelessController;

u16 Wireless_GetOperation(void)
{
    if (gWirelessController == 0)
        return 0;
    return gWirelessController->f2;
}
