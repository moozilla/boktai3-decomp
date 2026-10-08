#include "global.h"
struct S { u8 filler[0x294]; u32 *tbl; u8 f2[0x12]; u8 idx;};
void sub_0824923C(void *, u32);
void sub_08130AB4(struct S *p)
{
    u32 i = p->idx;
    sub_0824923C(p, p->tbl[i]);
}
