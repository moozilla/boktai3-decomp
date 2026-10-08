#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_081E27A8(void *);
void sub_0821A0C0(void *);
void sub_081E270C(void);
void sub_081E2794(void);

void *sub_081E2898(void)
{
    void *r = sub_08219FBC(0xb, 0x38);
    if (r != 0) {
        sub_0821A04C(r, sub_081E270C, sub_081E2794);
        if (sub_081E27A8(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
