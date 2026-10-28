#include "global.h"
#include "ram.h"

struct S {
    u8 f[0x33];
    u8 a;
};
u32 Wireless_GetStatus(void)
{
    struct S *p = gWirelessController;
    u32 r;
    if (p != 0)
        r = p->a;
    else
        r = gWirelessLastStatus;
    return r;
}
