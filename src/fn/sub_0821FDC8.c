#include "global.h"
struct Nd { struct Nd *next; u16 pad; u16 f6; };
struct M { u8 p[0x18]; struct Nd *f18; struct Nd *f1c; struct Nd *f20; u8 q[0xac]; u16 c0; u16 c1; u16 c2; };
extern struct M *gUnk_03001688;
void sub_0821FDC8(struct Nd *n)
{
    struct Nd *p;
    if (gUnk_03001688 == 0) return;
    p = gUnk_03001688->f18;
    while ((p = p->next) != 0)
        if (p == n) return;
    gUnk_03001688->f20->next = n;
    n->next = 0;
    gUnk_03001688->f20 = n;
    gUnk_03001688->c0++;
}
