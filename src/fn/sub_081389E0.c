#include "global.h"

extern u8 *gUnk_020004C0;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08138924(u8 *);
void sub_0821A0C0(u8 *);
void sub_08138778(void);
void sub_081388F4(void);

u8 *sub_081389E0(void)
{
    u8 *p;
    if (gUnk_020004C0 != 0)
        return gUnk_020004C0;
    p = sub_08219FBC(0xa, 0x19c);
    if (p != 0) {
        sub_0821A04C(p, sub_08138778, sub_081388F4);
        if (sub_08138924(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
