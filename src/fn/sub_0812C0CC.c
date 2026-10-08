#include "global.h"

extern u8 *gUnk_02000184;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0812C0A4(u8 *);
void sub_0821A0C0(u8 *);
void sub_0812BFBC(void);
void sub_0812C020(void);

u8 *sub_0812C0CC(void)
{
    u8 *p;
    if (gUnk_02000184 != 0)
        return gUnk_02000184;
    p = sub_08219FBC(0xa, 0xe1c);
    if (p != 0) {
        sub_0821A04C(p, sub_0812BFBC, sub_0812C020);
        if (sub_0812C0A4(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
