#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_081DDBC0(void);
void sub_081DE078(void);
s32 sub_081DE0CC(void *);

void *sub_081DE5A4(void)
{
    void *r = sub_08219FBC(0xb, 0x3e4);
    if (r != 0) {
        sub_0821A04C(r, sub_081DDBC0, sub_081DE078);
        if (sub_081DE0CC(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
