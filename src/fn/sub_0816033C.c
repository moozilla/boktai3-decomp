#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_081602D0(void *, void *);
void sub_0821A0C0(void *);
void sub_0815FF98(void);
void sub_0815FFB8(void);

void *sub_0816033C(void *p)
{
    void *r = sub_08219FBC(0xb, 0x4C8);
    if (r != 0) {
        sub_0821A04C(r, sub_0815FF98, sub_0815FFB8);
        if (sub_081602D0(r, p) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
