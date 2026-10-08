#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
void sub_0821A0C0(void *);
void sub_0804EA54(void);
void sub_0804EAFC(void);
s32 sub_0804EB60(void *, s32, s32);

void *sub_0804EB90(s32 a, s32 b)
{
    void *p = sub_08219FBC(9, 0xAB4);
    if (p) {
        sub_0821A04C(p, sub_0804EA54, sub_0804EAFC);
        if (sub_0804EB60(p, a, b) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
