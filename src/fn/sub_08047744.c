#include "global.h"

extern u8 gUnk_03005140[];
u8 *sub_0821A520(u32, u32);
void sub_08047744(void)
{
    CpuSet(sub_0821A520(0x92B3, 0xE39B) + 0x194, gUnk_03005140, 0x04000008);
}
