#include "global.h"

struct S {
    u8 f[0x60];
    void *a;
};
void WirelessGameDisplay_UpdateStatusIcons(void *);
void WirelessGameMenu_UpdateHealthIndicators(struct S *p)
{
    if (p->a)
        WirelessGameDisplay_UpdateStatusIcons(p->a);
}
