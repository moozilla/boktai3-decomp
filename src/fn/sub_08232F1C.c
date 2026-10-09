#include "global.h"
extern u8 *gUnk_02000098;
u8 *sub_08219FBC(s32, u32);
void sub_0821A04C(u8 *, void (*)(u8 *), void (*)(u8 *));
void sub_08232E6C(u8 *);
void sub_08232EA8(u8 *);
s32 sub_08232ED4(u8 *);
void sub_0821A0C0(u8 *);

u8 *sub_08232F1C(void)
{
    u8 *p;
    if (gUnk_02000098 != NULL) return gUnk_02000098;
    p = sub_08219FBC(11, 0x104);
    if (p != NULL) {
        sub_0821A04C(p, sub_08232E6C, sub_08232EA8);
        if (sub_08232ED4(p) < 0) {
            sub_0821A0C0(p);
            return NULL;
        }
    }
    return p;
}
