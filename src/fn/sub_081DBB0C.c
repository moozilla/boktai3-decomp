#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_081DB474(void);
void sub_081DB75C(void);
s32 sub_081DB794(void *);

void *sub_081DBB0C(void)
{
    void *r = sub_08219FBC(0xb, 0x304);
    if (r != 0) {
        sub_0821A04C(r, sub_081DB474, sub_081DB75C);
        if (sub_081DB794(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
