#include "global.h"

extern void *gUnk_02000244;
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_081A4DF0(void *, s32, s32, s32);
void sub_0821A0C0(void *);
void sub_081A4D8C(void);
void sub_081A4DB4(void);

void *sub_081A4EEC(s32 a, s32 b, s32 c)
{
    void *r;
    if (gUnk_02000244 != 0)
        return gUnk_02000244;
    gUnk_02000244 = sub_08219FBC(8, 0xEE4);
    r = gUnk_02000244;
    if (r != 0) {
        sub_0821A04C(r, sub_081A4D8C, sub_081A4DB4);
        if (sub_081A4DF0(r, a, b, c) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
