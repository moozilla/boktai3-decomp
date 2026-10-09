#include "global.h"

void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_0821159C(void *, void *);
void sub_0821A0C0(void *);
void sub_082112F4(void);
void sub_08211588(void);

void *sub_082116A8(void *a)
{
    void *p = sub_08219FBC(8, 0xba8);
    if (p != 0) {
        sub_0821A04C(p, sub_082112F4, sub_08211588);
        if (sub_0821159C(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
