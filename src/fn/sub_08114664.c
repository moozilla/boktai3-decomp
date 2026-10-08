#include "global.h"
s32 sub_082151E4(u8 *, u32);
void sub_082144A4(u8 *, u8 *, u32);
u32 sub_0821A520(u32, u32);
void sub_08220C8C(u8 *, u32, u32, u32, u32);
void sub_08215284(u8 *, u32);
void sub_08114664(u8 *p)
{
    u8 *q = p + 0x44;
    if (sub_082151E4(q, 0x409C) != 0) {
        u32 r;
        u8 *t;
        sub_082144A4(p + 0x60, q, 0);
        r = sub_0821A520(0x922E, 0x1C22);
        *(u32 *)(p + 0x9c) = r;
        sub_08220C8C(p + 0x8c, r, 5, 0, 0);
        sub_08215284(q, 0xe4);
    }
}
