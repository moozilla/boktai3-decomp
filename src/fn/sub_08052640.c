#include "global.h"

struct S {
    u8 filler[0x50];
    u32 flags;
    u8 filler2[0x28];
    u8 sub[0x148];
    u16 v;
};
void AnimData_SelectTiles(u8 *, u16);

void sub_08052640(struct S *p)
{
    p->flags |= 1;
    AnimData_SelectTiles(p->sub, p->v);
}
