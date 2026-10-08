#include "global.h"
s32 sub_082151E4(void *, u32);
void sub_082144A4(void *, void *, u32);
u32 sub_0821A520(u32, u32);
void sub_08215284(void *, u32);
void sub_08220C8C(void *, u32, u32, u32, u32);
void sub_081146C0(u8 *s, s32 x)
{
    u8 *p = s + 0x44;
    if (sub_082151E4(p, 0x409c) != 0) {
        u32 r;
        u8 *q;
        sub_082144A4(s + 0x60, p, 0);
        r = sub_0821A520(0x922e, 0x1c22);
        q = s + 0x9c;
        *(u32 *)q = r;
        if (x > 0x3c) {
            sub_08220C8C(q - 0x10, r, 5, 2, 0);
        } else {
            sub_08220C8C(s + 0x8c, r, 5, 0, 0);
        }
        sub_08215284(s + 0x44, 0xe4);
    }
}
