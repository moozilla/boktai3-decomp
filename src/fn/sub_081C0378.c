#include "global.h"
struct S {
    u8 f[0x18];
    u8 g[4];
    u8 b1c;
};
void Anim_Remove(void *);
void sub_081C0378(struct S *p)
{
    if (p->b1c != 0)
        Anim_Remove(p->f + 0x18);
}
