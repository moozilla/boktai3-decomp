#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_081F26BC(void);
void sub_081F2700(void);
s32 sub_081F273C(void *);

void *sub_081F2764(void)
{
    void *r = sub_08219FBC(8, 0x980);
    if (r != 0) {
        sub_0821A04C(r, sub_081F26BC, sub_081F2700);
        if (sub_081F273C(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
