#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_081DC244(void);
void sub_081DC448(void);
s32 sub_081DC470(void *);

void *sub_081DC690(void)
{
    void *r = sub_08219FBC(0xb, 0x16c);
    if (r != 0) {
        sub_0821A04C(r, sub_081DC244, sub_081DC448);
        if (sub_081DC470(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
