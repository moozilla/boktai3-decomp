#include "global.h"

extern u8 *gUnk_02000200;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0811B714(u8 *);
void sub_0821A0C0(u8 *);
void sub_0811B5F0(void);
void sub_0811B668(void);

u8 *sub_0811B72C(void)
{
    u8 *p;
    if (gUnk_02000200 != 0)
        return gUnk_02000200;
    p = sub_08219FBC(5, 0xc0);
    if (p != 0) {
        sub_0821A04C(p, sub_0811B5F0, sub_0811B668);
        if (sub_0811B714(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
