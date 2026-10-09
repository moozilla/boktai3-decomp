#include "global.h"

extern void *gUnk_02000238;
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_081A3600(void *, s32, s32, s32);
void sub_0821A0C0(void *);
void sub_081A3430(void);
void sub_081A3468(void);

void *sub_081A3710(s32 a, s32 b, s32 c)
{
    void *r;
    if (gUnk_02000238 != 0)
        return gUnk_02000238;
    gUnk_02000238 = sub_08219FBC(8, 0x1248);
    r = gUnk_02000238;
    if (r != 0) {
        sub_0821A04C(r, sub_081A3430, sub_081A3468);
        if (sub_081A3600(r, a, b, c) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
