#include "global.h"

void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08008174(void *, void *);
void sub_0821A0C0(void *);
void sub_08007ECC(void);
void sub_08008148(void);

void *sub_08008230(void *a)
{
    void *p = sub_08219FBC(9, 0x5c);
    if (p != 0) {
        sub_0821A04C(p, sub_08007ECC, sub_08008148);
        if (sub_08008174(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
