#include "global.h"
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0811A6C4(u8 *);
void sub_0821A0C0(u8 *);
void sub_0811A604(void);
void sub_0811A62C(void);

u8 *sub_0811AB38(void)
{
    u8 *p;
    p = sub_08219FBC(6, 0x968);
    if (p != 0) {
        sub_0821A04C(p, sub_0811A604, sub_0811A62C);
        if (sub_0811A6C4(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
