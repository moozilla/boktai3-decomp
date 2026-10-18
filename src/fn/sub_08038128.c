#include "global.h"

struct S {
    u8 f[0x60];
    void *a;
};
void WirelessGameDisplay_SetState(void *);
void WirelessGameMenu_SetStatusPanelState(struct S *p)
{
    if (p->a)
        WirelessGameDisplay_SetState(p->a);
}
