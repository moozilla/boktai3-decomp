#include "global.h"

struct S { u8 pad[0x18]; u32 f18; u32 pad1c; u32 f20; u8 b24; };
u32 sub_081D3F24(struct S *);
void sub_081D4DE4(struct S *, u32);
void sub_081D4A34(struct S *);

void sub_081D3FFC(struct S *p)
{
    u32 z;
    sub_081D4DE4(p, sub_081D3F24(p));
    z = 0;
    p->f18 = z;
    p->f20 = z;
    p->b24 = z;
    sub_081D4A34(p);
}
