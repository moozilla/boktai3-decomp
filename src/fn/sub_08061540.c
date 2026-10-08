#include "global.h"

struct S { u8 f[0x1AA4]; u8 a; u8 g[0x1AB1-0x1AA5]; u8 b; u8 h[0x1AEC-0x1AB2]; u16 c; };

u16 sub_0806143C(struct S *);
void sub_080613BC(struct S *, s32);
void sub_0806130C(struct S *, void (*)(void));
void sub_08061DA0(void);

void sub_08061540(struct S *p)
{
    p->a = 0x40;
    if (p->b > 1) p->a = 0x41;
    p->c = sub_0806143C(p);
    sub_080613BC(p, 0);
    sub_0806130C(p, sub_08061DA0);
}
