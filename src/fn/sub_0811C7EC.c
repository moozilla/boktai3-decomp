#include "global.h"
extern u8 *gUnk_030042E4;
void CpuSet(const void *, void *, u32);
s32 sub_0811C77C(u8 *);
void sub_0811C7EC(u8 *s)
{
    s32 i = sub_0811C77C(s);
    CpuSet(gUnk_030042E4 + (i << 5), s + 0x210, 0x10);
}
