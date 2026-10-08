#include "global.h"

extern u32 gUnk_0300523C;
struct A { u8 f[0x28]; u16 v; u8 g[0x38]; u16 c; };
void sub_0821A0C0(void *);

void sub_081AF5EC(struct A *p)
{
    u32 t = gUnk_0300523C & 7;
    if (t == 0) {
        p->v = (p->c >> 4) + 6;
        p->c++;
        if (p->c == 0x50) {
            p->c = t;
            sub_0821A0C0(p);
        }
    }
}
