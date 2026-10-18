#include "global.h"
struct Display {
    u8 pad0[0x1c];
    u8 state;
};
extern void *gUnk_086057FC[];
int Wireless_GetStatus(void);
void sub_0824923C(void *, void *);
int WirelessGameDisplay_Update(struct Display *p)
{
    if (!Wireless_GetStatus())
        sub_0824923C(p, gUnk_086057FC[p->state]);
    return 0;
}
