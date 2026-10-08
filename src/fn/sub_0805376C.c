#include "global.h"
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_080532B0(u8 *, u32, u32);
void sub_0821A0C0(u8 *);
void sub_08053178(void);
void sub_080531BC(void);
u8 *sub_0805376C(u32 a, u32 b)
{
    u8 *r = sub_08219FBC(9, 0x388);
    if (r) {
        sub_0821A04C(r, (void (*)(void))sub_08053178, (void (*)(void))sub_080531BC);
        if (sub_080532B0(r, a, b) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
