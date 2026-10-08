#include "global.h"

u32 sub_0821A520(u32, u32);

void sub_081C6280(u8 *p)
{
    u32 r = sub_0821A520(0x92B3, 0x204);
    p += 0x360;
    r += 0x14;
    *(u32 *)p = r;
    CpuSet((void *)r, (void *)0x03004FC0, 0x04000028);
}
