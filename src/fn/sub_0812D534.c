#include "global.h"

extern u8 *gUnk_02000190;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0812D50C(u8 *);
void sub_0821A0C0(u8 *);
void sub_0812D46C(void);
void sub_0812D4CC(void);

u8 *sub_0812D534(void)
{
    u8 *p;
    if (gUnk_02000190 != 0)
        return gUnk_02000190;
    p = sub_08219FBC(0xa, 0x53c);
    if (p != 0) {
        sub_0821A04C(p, sub_0812D46C, sub_0812D4CC);
        if (sub_0812D50C(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
