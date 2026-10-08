#include "global.h"

u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08109A04(u8 *);
void sub_0821A0C0(u8 *);
void sub_081099A4(void);
void sub_081099E4(void);

u8 *sub_08109AB0(void)
{
    u8 *p;
    p = sub_08219FBC(11, 0x4C);
    if (p != 0) {
        sub_0821A04C(p, sub_081099A4, sub_081099E4);
        if (sub_08109A04(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
