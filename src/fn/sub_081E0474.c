#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_081E013C(void *);
void sub_0821A0C0(void *);
void sub_081DFCB4(void);
void sub_081E00E8(void);

void *sub_081E0474(void)
{
    void *r = sub_08219FBC(0xb, 2076);
    if (r != 0) {
        sub_0821A04C(r, sub_081DFCB4, sub_081E00E8);
        if (sub_081E013C(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
