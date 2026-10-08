#include "global.h"

struct A { u8 f[0xe0]; u16 n; };
void sub_082260A4(s32);
void sub_081A4A78(void *);

void sub_081A4BC8(struct A *p)
{
    u16 *q = &p->n;
    s32 d = 8 - *q;
    sub_082260A4(d * d);
    if (*q > 7)
        sub_081A4A78(p);
    *q += 1;
}
