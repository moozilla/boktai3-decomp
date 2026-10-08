#include "global.h"

struct S {
    u8 filler[0x50];
    u32 flags;
    u8 filler2[0x28];
    u8 sub[0x148];
    u16 v;
};
void sub_08215284(u8 *, u16);

void sub_08052640(struct S *p)
{
    p->flags |= 1;
    sub_08215284(p->sub, p->v);
}
