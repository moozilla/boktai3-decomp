#include "global.h"
extern u8 *gUnk_02000124;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08066C0C(u8 *);
void sub_0821A0C0(u8 *);
void sub_08066AE0(void);
void sub_08066B28(void);
u8 *sub_08066C34(void)
{
    u8 *r;
    if (gUnk_02000124 != 0) return gUnk_02000124;
    r = sub_08219FBC(9, 0xa20);
    if (r) {
        sub_0821A04C(r, sub_08066AE0, sub_08066B28);
        if (sub_08066C0C(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
