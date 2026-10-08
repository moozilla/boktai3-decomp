#include "global.h"
extern u8 *gUnk_030042E4;
void CpuSet(const void *, void *, u32);
void sub_0811C818(u8 *a, s32 b)
{
    u8 **gp;
    u32 k = b + 0x379;
    gp = &gUnk_030042E4;
    CpuSet(*gp + (k << 5), a + 0x230, 0x10);
}
