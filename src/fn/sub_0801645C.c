#include "global.h"
extern u32 gUnk_0200006C;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_080163C8(void *, void *);
void sub_0821A0C0(void *);
void sub_08016334(void);
void sub_0801636C(void);

void *sub_0801645C(void *a)
{
    void *p;
    if (gUnk_0200006C != 0)
        return (void *)gUnk_0200006C;
    p = sub_08219FBC(10, 0xcfc);
    if (p != 0) {
        sub_0821A04C(p, sub_08016334, sub_0801636C);
        if (sub_080163C8(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
