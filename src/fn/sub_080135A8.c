#include "global.h"

void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08013584(void *, void *);
void sub_0821A0C0(void *);
void sub_08013558(void);
void sub_08013578(void);

void *sub_080135A8(void *a)
{
    void *p = sub_08219FBC(9, 0x20);
    if (p != 0) {
        sub_0821A04C(p, sub_08013558, sub_08013578);
        if (sub_08013584(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
