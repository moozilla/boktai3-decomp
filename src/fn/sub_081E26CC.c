#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_081E23A0(void *);
void sub_0821A0C0(void *);
void sub_081E2154(void);
void sub_081E2300(void);

void *sub_081E26CC(void)
{
    void *r = sub_08219FBC(0xb, 1572);
    if (r != 0) {
        sub_0821A04C(r, sub_081E2154, sub_081E2300);
        if (sub_081E23A0(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
