#include "global.h"

extern u8 *gUnk_02000170;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08128AB8(u8 *);
void sub_0821A0C0(u8 *);
void sub_08128A00(void);
void sub_08128A5C(void);

u8 *sub_08128AF0(void)
{
    u8 *p;
    if (gUnk_02000170 != 0)
        return gUnk_02000170;
    p = sub_08219FBC(0xa, 0x5bc);
    if (p != 0) {
        sub_0821A04C(p, sub_08128A00, sub_08128A5C);
        if (sub_08128AB8(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
