#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_081EB5A8(void *);
void sub_0821A0C0(void *);
void sub_081EAB64(void);
void sub_081EB598(void);

void *sub_081EB7DC(void)
{
    void *r = sub_08219FBC(0xb, 1900);
    if (r != 0) {
        sub_0821A04C(r, sub_081EAB64, sub_081EB598);
        if (sub_081EB5A8(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
