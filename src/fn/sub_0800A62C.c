#include "global.h"

void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_0800A534(void *, void *);
void sub_0821A0C0(void *);
void sub_0800A3A8(void);
void sub_0800A500(void);

void *sub_0800A62C(void *a)
{
    void *p = sub_08219FBC(10, 0xA2C);
    if (p != 0) {
        sub_0821A04C(p, sub_0800A3A8, sub_0800A500);
        if (sub_0800A534(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
