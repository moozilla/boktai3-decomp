#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_081EB8E0(void);
void sub_081EBCF4(void);
s32 sub_081EBD0C(void *);

void *sub_081EC168(void)
{
    void *r = sub_08219FBC(0xb, 0x214);
    if (r != 0) {
        sub_0821A04C(r, sub_081EB8E0, sub_081EBCF4);
        if (sub_081EBD0C(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
