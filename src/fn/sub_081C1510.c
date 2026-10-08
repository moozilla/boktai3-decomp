#include "global.h"
struct Q { u32 a[8]; };
struct S { u8 f[0x18]; struct Q q1; struct Q q2; };
struct Q *sub_0821A520(u32, u32);
void sub_082196C4(void *, void *);
void sub_081C1434(void *);
void sub_081C1510(struct S *p)
{
    struct Q *r;
    u32 id = 0xCB05;
    r = sub_0821A520(id, 0xE2AB);
    p->q1 = *r;
    sub_082196C4(&p->q1, r);
    r = sub_0821A520(id, 0x530D);
    p->q2 = *r;
    sub_082196C4(&p->q2, r);
    sub_081C1434(p);
}
