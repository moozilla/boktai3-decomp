#include "global.h"
struct Nd { struct Nd *next; u16 pad; u16 f6; };
struct M { u8 p[0x18]; struct Nd *f18; struct Nd *f1c; struct Nd *f20; u8 q[0xac]; u16 c0; u16 c1; u16 c2; };
extern struct M *gUnk_03001688;
void sub_0821FE6C(struct Nd *n)
{
    struct Nd *prev, *cur;
    if (gUnk_03001688 == 0) return;
    prev = gUnk_03001688->f18;
    cur = prev->next;
    while (cur) {
        if (cur == n) {
            if (cur == gUnk_03001688->f20) gUnk_03001688->f20 = prev;
            prev->next = cur->next;
            gUnk_03001688->c0--;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
}
