#include "global.h"

extern u8 *gUnk_020004D4;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0813A24C(u8 *);
void sub_0821A0C0(u8 *);
void sub_0813A1E0(void);
void sub_0813A228(void);

u8 *sub_0813A300(void)
{
    u8 *p;
    if (gUnk_020004D4 != 0)
        return gUnk_020004D4;
    p = sub_08219FBC(0xa, 0x1d4);
    if (p != 0) {
        sub_0821A04C(p, sub_0813A1E0, sub_0813A228);
        if (sub_0813A24C(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
