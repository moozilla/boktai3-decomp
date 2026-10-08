#include "global.h"
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_080506CC(u8 *, u32, u32);
void sub_0821A0C0(u8 *);
void sub_08050694(void);
void sub_080506B8(void);
u8 *sub_080507DC(u32 a, u32 b)
{
    u8 *r = sub_08219FBC(8, 0x3c);
    if (r) {
        sub_0821A04C(r, (void (*)(void))sub_08050694, (void (*)(void))sub_080506B8);
        if (sub_080506CC(r, a, b) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
