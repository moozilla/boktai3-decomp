#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void *), void (*)(void *));
s32 sub_0811B7DC(void *);
void sub_0821A0C0(void *);
void sub_0811B7A0(void *);
void sub_0811B7D0(void *);
void *sub_0811B7E8(void)
{
    void *p = sub_08219FBC(0x8, 0x378);
    if (p != 0) {
        sub_0821A04C(p, sub_0811B7A0, sub_0811B7D0);
        if (sub_0811B7DC(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
