#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
void sub_0821A0C0(void *);
void sub_0804FF48(void);
void sub_0804FF70(void);
s32 sub_0804FFDC(void *);

void *sub_0804FFEC(void)
{
    void *p = sub_08219FBC(9, 0x7FC);
    if (p) {
        sub_0821A04C(p, sub_0804FF48, sub_0804FF70);
        if (sub_0804FFDC(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
