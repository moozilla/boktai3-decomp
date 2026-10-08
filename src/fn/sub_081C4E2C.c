#include "global.h"

struct S { u8 pad[0x18]; u32 f18; };
u32 sub_081C3010(u32);
void sub_081C4D58(struct S *);

void sub_081C4E2C(struct S *p)
{
    if (sub_081C3010(p->f18)) sub_081C4D58(p);
}
