#include "global.h"

struct S { u8 pad[0x34]; u16 f34; };
void sub_081C4C18(struct S *);

void sub_081C4EC0(struct S *p)
{
    p->f34++;
    if (p->f34 > 0x40) sub_081C4C18(p);
}
