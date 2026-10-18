#include "global.h"
struct S {
    u8 f[0x18];
    u32 fl;
};
extern struct S *gUnk_0200026C;
void ShipWheel_RequestStop(void)
{
    struct S *p = gUnk_0200026C;
    if (p != 0)
        p->fl |= 1;
}
