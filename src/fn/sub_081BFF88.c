#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_081BFF18(void *);
void sub_0821A0C0(void *);
void sub_081BFEAC(void);
void sub_081BFEDC(void);

void *sub_081BFF88(void)
{
    void *r = sub_08219FBC(0xb, 6136);
    if (r != 0) {
        sub_0821A04C(r, sub_081BFEAC, sub_081BFEDC);
        if (sub_081BFF18(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
