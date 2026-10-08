#include "global.h"

extern u8 *gUnk_02000148;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08125B00(u8 *);
void sub_0821A0C0(u8 *);
void sub_08125A94(void);
void sub_08125AF4(void);

u8 *sub_08125B0C(void)
{
    u8 *p;
    if (gUnk_02000148 != 0)
        return gUnk_02000148;
    p = sub_08219FBC(0xa, 0x1e4);
    if (p != 0) {
        sub_0821A04C(p, sub_08125A94, sub_08125AF4);
        if (sub_08125B00(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
