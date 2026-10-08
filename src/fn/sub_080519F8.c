#include "global.h"
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08051858(u8 *, u32, u32);
void sub_0821A0C0(u8 *);
void sub_080516FC(void);
void sub_0805173C(void);
u8 *sub_080519F8(u32 a, u32 b)
{
    u8 *r = sub_08219FBC(8, 0xd0);
    if (r) {
        sub_0821A04C(r, (void (*)(void))sub_080516FC, (void (*)(void))sub_0805173C);
        if (sub_08051858(r, a, b) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
