#include "global.h"
struct S { u8 filler[0x29C]; u32 *tbl; u8 f2[0xA]; u8 idx;};
void sub_0824923C(void *, u32);
void sub_08130AD8(struct S *p)
{
    u32 i = p->idx;
    sub_0824923C(p, p->tbl[i]);
}
