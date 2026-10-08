#include "global.h"

struct S { u8 pad[0x2e2]; u8 f2e2; };
u8 sub_081C55B8(struct S *);
void sub_081C569C(struct S *);

void sub_081C5894(struct S *p)
{
    if (sub_081C55B8(p)) p->f2e2 = 0;
    sub_081C569C(p);
}
