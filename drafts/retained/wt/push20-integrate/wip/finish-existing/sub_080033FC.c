#include "global.h"
extern u8 *gUnk_02000468;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_080031EC(u8 *);
void sub_0821A0C0(u8 *);
void sub_080031AC(void);
void sub_080031E0(void);
u8 *sub_080033FC(void)
{
    u8 *r;
    if (gUnk_02000468 != 0) return gUnk_02000468;
    r = sub_08219FBC(11, 0xe94);
    if (r) {
        sub_0821A04C(r, sub_080031AC, sub_080031E0);
        if (sub_080031EC(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
