#include "global.h"

struct S { u8 pad[0x34]; u16 f34; s16 f36; };
void sub_081C4C3C(struct S *);

void sub_081C4EA0(struct S *p)
{
    p->f34++;
    if (p->f34 > p->f36) sub_081C4C3C(p);
}
