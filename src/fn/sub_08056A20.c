#include "global.h"
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08056A00(u8 *);
void sub_0821A0C0(u8 *);
void sub_08056954(void);
void sub_080569D8(void);
u8 *sub_08056A20(void)
{
    u8 *r = sub_08219FBC(9, 0x2c8);
    if (r) {
        sub_0821A04C(r, sub_08056954, sub_080569D8);
        if (sub_08056A00(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
