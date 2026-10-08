#include "global.h"
void sub_082151E4(u8 *, u32);
void sub_082144A4(u8 *, u8 *, u32);
void sub_08215284(u8 *, u32);
void sub_080573F8(u8 *p)
{
    u8 *q = p + 0x44;
    sub_082151E4(q, 0x1C1D);
    sub_082144A4(p + 0x18, q, 0);
    sub_08215284(q, 1);
    *(u32 *)(p + 0x18) |= 1;
    p[0x1f] = 1;
}
