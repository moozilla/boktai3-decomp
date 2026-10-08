#include "global.h"

extern u8 *gUnk_02000198;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0806E224(u8 *);
void sub_0821A0C0(u8 *);
void sub_0806E130(void);
void sub_0806E1C0(void);

u8 *sub_0806E250(void)
{
    u8 *p;
    if (gUnk_02000198 != 0)
        return gUnk_02000198;
    p = sub_08219FBC(10, 0x600);
    if (p != 0) {
        sub_0821A04C(p, sub_0806E130, sub_0806E1C0);
        if (sub_0806E224(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
