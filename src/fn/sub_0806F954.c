#include "global.h"

u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0806F870(u8 *);
void sub_0821A0C0(u8 *);
void sub_0806F838(void);
void sub_0806F86C(void);

u8 *sub_0806F954(void)
{
    u8 *p;
    p = sub_08219FBC(8, 0x2c);
    if (p != 0) {
        sub_0821A04C(p, sub_0806F838, sub_0806F86C);
        if (sub_0806F870(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
