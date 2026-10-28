#include "global.h"
#include "ram.h"

struct S {
    u8 f[0x1d];
    u8 a;
};
void Wireless_RequestDestroyController(void)
{
    struct S *p = gWirelessController;
    if (p)
        p->a = 1;
}
