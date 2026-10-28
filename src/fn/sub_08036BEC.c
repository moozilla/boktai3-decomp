#include "global.h"
#include "ram.h"

struct S {
    u8 f[0x33];
    u8 a;
};
extern struct S *gUnk_02000484;
u32 Wireless_GetStatus(void)
{
    struct S *p = gUnk_02000484;
    u32 r;
    if (p != 0)
        r = p->a;
    else
        r = gWirelessLastStatus;
    return r;
}
