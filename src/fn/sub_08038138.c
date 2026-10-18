#include "global.h"

struct S {
    u8 f[0x60];
    void *a;
};
void WirelessGameDisplay_RequestCompletion(void *);
void WirelessGameMenu_SignalStatusPanel(struct S *p)
{
    if (p->a)
        WirelessGameDisplay_RequestCompletion(p->a);
}
