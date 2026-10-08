#include "global.h"
extern u32 gUnk_020004F0[];
u32 sub_0821ABA8(u32, u32);
void sub_08055F3C(u32, u32, void (*)(void), u32);
void sub_08119F9C(void);
void sub_0811A004(void)
{
    u32 p;
    u32 i = sub_0821ABA8(0x69, 0);
    p = gUnk_020004F0[i];
    if (p != 0) sub_08055F3C(0, 0, sub_08119F9C, p);
}
