#include "global.h"
s32 sub_082151E4(void *, u32);
void sub_082144A4(void *, void *, u32);
u32 sub_0821A520(u32, u32);
void sub_08215284(void *, u32);
void sub_08220C8C(void *, u32, u32, u32, u32);
void sub_0811473C(u8 *s, s32 x)
{
    u8 *p = s + 0x44;
    if (sub_082151E4(p, 0x409c) != 0) {
        u32 r;
        u8 *q;
        sub_082144A4(s + 0x60, p, 0);
        r = sub_0821A520(0x922e, 0x1c22);
        q = s + 0x9c;
        *(u32 *)q = r;
        sub_08220C8C(q - 0x10, r, 5, 1, 0);
        sub_08215284(p, 0xe4);
        if (x == 0) {
            *(u8 *)(s + 0xca) = 0xe0;
            *(s32 *)(s + 0xd0) = 1;
        } else {
            u8 *b = s + 0xca;
            s32 c = 0x20;
            *b = c;
            *(s32 *)(b + 6) = c - 0x21;
        }
    }
}
