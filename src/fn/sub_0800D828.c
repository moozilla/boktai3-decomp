#include "global.h"

extern void *gUnk_0200046C;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_0800D800(void *, void *);
void sub_0821A0C0(void *);
void sub_0800D668(void);
void sub_0800D7D4(void);

void *sub_0800D828(void *a)
{
    void *p;
    if (gUnk_0200046C != 0)
        return gUnk_0200046C;
    p = sub_08219FBC(8, 0x20);
    if (p != 0) {
        sub_0821A04C(p, sub_0800D668, sub_0800D7D4);
        if (sub_0800D800(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
