#include "global.h"
u32 sub_0821A520(u32, u32);
void sub_08220C8C(u8 *, u32, u32, u32, u32);
void sub_0812DA2C(u8 *p, u16 b)
{
    u32 r = sub_0821A520(0x922E, b);
    *(u32 *)(p + 0x58) = r;
    sub_08220C8C(p + 0x48, r, 0, 0, 0);
}
