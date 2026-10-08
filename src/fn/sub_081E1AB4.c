#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_081E132C(void);
void sub_081E154C(void);
s32 sub_081E15C8(void *);

void *sub_081E1AB4(void)
{
    void *r = sub_08219FBC(0xb, 0x4f0);
    if (r != 0) {
        sub_0821A04C(r, sub_081E132C, sub_081E154C);
        if (sub_081E15C8(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
