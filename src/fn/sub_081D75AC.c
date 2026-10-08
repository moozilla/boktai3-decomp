#include "global.h"

u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_081D750C(u8 *);
void sub_0821A0C0(u8 *);
void sub_081D7434(void);
void sub_081D7508(void);

u8 *sub_081D75AC(void)
{
    u8 *p;
    p = sub_08219FBC(11, 0x20);
    if (p != 0) {
        sub_0821A04C(p, sub_081D7434, sub_081D7508);
        if (sub_081D750C(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
