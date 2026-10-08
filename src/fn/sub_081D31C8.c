#include "global.h"

struct S { u8 pad[0x3c]; u32 f3c; };

void sub_081D31C8(struct S *p)
{
    if (p) p->f3c = 0;
}
