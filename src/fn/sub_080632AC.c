#include "global.h"

struct P { u8 f[0x1aab]; u8 a; u8 g[0x1aee - 0x1aac]; u16 n; u8 h[0x1cf8 - 0x1af0]; void *cb; };
void sub_0805DF08(u8);
void sub_0805DE3C(u8);
void sub_0824923C(void *, void *);

s32 sub_080632AC(struct P *p)
{
    u8 *a = &p->a;
    sub_0805DF08(*a);
    sub_0805DE3C(*a);
    sub_0824923C(p, p->cb);
    p->n++;
    return 0;
}
