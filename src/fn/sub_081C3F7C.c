#include "global.h"

struct S { u8 pad[0xf4]; u32 f4; };
void sub_081C3EF0(struct S *);
void sub_081C3950(struct S *);
void sub_0824923C(struct S *, u32);

void sub_081C3F7C(struct S *p)
{
    sub_081C3EF0(p);
    if (p->f4) {
        sub_081C3950(p);
        sub_0824923C(p, p->f4);
    }
}
