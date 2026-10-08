#include "global.h"

extern u8 *gUnk_02000194;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0812D9A8(u8 *);
void sub_0821A0C0(u8 *);
void sub_0812D8EC(void);
void sub_0812D960(void);

u8 *sub_0812D9D4(void)
{
    u8 *p;
    if (gUnk_02000194 != 0)
        return gUnk_02000194;
    p = sub_08219FBC(0xa, 0x6a0);
    if (p != 0) {
        sub_0821A04C(p, sub_0812D8EC, sub_0812D960);
        if (sub_0812D9A8(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
