#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_081EC1A8(void);
void sub_081EC1F8(void);
s32 sub_081EC204(void *);

void *sub_081EC340(void)
{
    void *r = sub_08219FBC(0xb, 0x138);
    if (r != 0) {
        sub_0821A04C(r, sub_081EC1A8, sub_081EC1F8);
        if (sub_081EC204(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
