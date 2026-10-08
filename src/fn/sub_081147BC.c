#include "global.h"
s32 sub_082151E4(void *, u32);
void sub_082144A4(void *, void *, u32);
u32 sub_0821A520(u32, u32);
void sub_08215284(void *, u32);
void sub_081147BC(u8 *s)
{
    u8 *p = s + 0x44;
    if (sub_082151E4(p, 0x409c) != 0) {
        sub_082144A4(s + 0x60, p, 0);
        *(u32 *)(s + 0x9c) = sub_0821A520(0x922e, 0x1c22);
        sub_08215284(p, 0xe4);
    }
}
