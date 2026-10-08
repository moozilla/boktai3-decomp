#include "global.h"
extern void *gUnk_02000264;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void *), void (*)(void *));
s32 sub_081C2D68(void *, void *);
void sub_0821A0C0(void *);
void sub_081C2D0C(void *);
void sub_081C2D34(void *);
void *sub_081C2FC0(void *a)
{
    void *p;
    if (gUnk_02000264 != 0) return gUnk_02000264;
    p = gUnk_02000264 = sub_08219FBC(8, 0x780);
    if (p != 0) {
        sub_0821A04C(p, sub_081C2D0C, sub_081C2D34);
        if (sub_081C2D68(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
