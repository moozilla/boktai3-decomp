#include "global.h"
#include "ram.h"
struct S {
    u8 f[0x18];
    u32 fl;
};
void ShipWheel_RequestStop(void)
{
    struct S *p = gShipWheel;
    if (p != 0)
        p->fl |= 1;
}
