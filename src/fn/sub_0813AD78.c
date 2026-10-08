#include "global.h"

extern u8 *gUnk_020001E0;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_0813AD60(u8 *);
void sub_0821A0C0(u8 *);
void sub_0813AC78(void);
void sub_0813AD08(void);

u8 *sub_0813AD78(void)
{
    u8 *p;
    if (gUnk_020001E0 != 0)
        return gUnk_020001E0;
    p = sub_08219FBC(0xa, 0x99c);
    if (p != 0) {
        sub_0821A04C(p, sub_0813AC78, sub_0813AD08);
        if (sub_0813AD60(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
