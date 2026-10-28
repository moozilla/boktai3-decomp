#include "global.h"
#include "ram.h"

struct S {
    u8 f[0xfb];
    u8 a;
};
void Wireless_RequestInputResync(void)
{
    struct S *p = gWirelessController;
    if (p)
        p->a = 1;
}
