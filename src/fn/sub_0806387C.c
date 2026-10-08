#include "global.h"

void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_0806386C(void *, void *);
void sub_0821A0C0(void *);
void sub_0806385C(void);
void sub_08063868(void);

void *sub_0806387C(void *a)
{
    void *p = sub_08219FBC(11, 0x50);
    if (p != 0) {
        sub_0821A04C(p, sub_0806385C, sub_08063868);
        if (sub_0806386C(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
