#include "global.h"

struct S { u8 pad[0x34]; u16 f34; };
void sub_081C4CF0(struct S *);

void sub_081C4E10(struct S *p)
{
    p->f34++;
    if (p->f34 > 0x3c) sub_081C4CF0(p);
}
