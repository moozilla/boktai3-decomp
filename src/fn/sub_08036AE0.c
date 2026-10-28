#include "global.h"
#include "ram.h"

struct S {
    u8 pad0[56];
    u16 f2;
    u32 f4;
    u32 f8;
};

u16 Wireless_GetOperation(void)
{
    if (((struct S *)gWirelessController) == 0)
        return 0;
    return ((struct S *)gWirelessController)->f2;
}
