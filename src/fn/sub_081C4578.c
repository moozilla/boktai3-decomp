#include "global.h"

struct S { u8 pad[0x4d]; u8 f4d; u8 pad2[0x68-0x4e]; s32 f68; };
void sub_081C439C(struct S *);
void sub_081B95E4(u8 *);
void sub_081C43B0(struct S *);

void sub_081C4578(struct S *p)
{
    s32 r;
    sub_081C439C(p);
    sub_081B95E4((u8 *)p + 0x24);
    r = 0;
    if (p->f4d == 2) r = 1;
    if (r) sub_081C43B0(p);
    p->f68++;
}
