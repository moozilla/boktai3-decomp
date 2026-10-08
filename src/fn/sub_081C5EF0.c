#include "global.h"

struct S { u8 pad[0x36c]; u32 f36c; u32 f370; };

void sub_081C5EF0(struct S *p, u32 v)
{
    p->f36c = v;
    p->f370 = 0;
}
