#include "global.h"
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08063334(u8 *);
void sub_0821A0C0(u8 *);
void sub_080632AC(void);
void sub_080632EC(void);
u8 *sub_080634D4(void)
{
    u8 *r = sub_08219FBC(0xB, 0x1D24);
    if (r) {
        sub_0821A04C(r, sub_080632AC, sub_080632EC);
        r[0x1AA7] = 1;
        r[0x1AA8] = 1;
        if (sub_08063334(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
