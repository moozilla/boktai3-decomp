#include "global.h"

u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08106D20(u8 *);
void sub_0821A0C0(u8 *);
void sub_08106BCC(void);
void sub_08106C88(void);

u8 *sub_08106D64(void)
{
    u8 *p;
    p = sub_08219FBC(10, 0xA8);
    if (p != 0) {
        sub_0821A04C(p, sub_08106BCC, sub_08106C88);
        if (sub_08106D20(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
