#include "global.h"
extern u8 gUnk_03005160[];
u8 *sub_0821A520(u32, u32);
void CpuSet(const void *, void *, u32);
void sub_081BF634(u8 *p)
{
    u8 *r = sub_0821A520(0x92B3, 0x81B);
    *(u8 **)(p + 0x1730) = r + 0x1b4;
    CpuSet(r + 0x1b4, gUnk_03005160, 0x04000018);
    *(u8 *)(p + 0x17C4) = 0;
}
