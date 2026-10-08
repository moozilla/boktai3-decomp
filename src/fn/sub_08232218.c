#include "global.h"

extern void *gUnk_0200005C;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_082321E0(void *, void *);
void sub_0821A0C0(void *);
void sub_082321A4(void);
void sub_082321D4(void);

void *sub_08232218(void *a)
{
    void *p;
    if (gUnk_0200005C != 0)
        return gUnk_0200005C;
    p = sub_08219FBC(9, 0x24);
    if (p != 0) {
        sub_0821A04C(p, sub_082321A4, sub_082321D4);
        if (sub_082321E0(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
