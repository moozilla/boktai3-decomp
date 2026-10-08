#include "global.h"

extern u8 *gUnk_020001E4;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08115924(u8 *);
void sub_0821A0C0(u8 *);
void sub_081158B8(void);
void sub_081158FC(void);

u8 *sub_081159D4(void)
{
    u8 *p;
    if (gUnk_020001E4 != 0)
        return gUnk_020001E4;
    p = sub_08219FBC(0x8, 0x2e98);
    if (p != 0) {
        sub_0821A04C(p, sub_081158B8, sub_081158FC);
        if (sub_08115924(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
