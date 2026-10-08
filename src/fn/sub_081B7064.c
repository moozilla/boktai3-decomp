#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_081B7038(void *, s32);
void sub_0821A0C0(void *);
void sub_081B6FA8(void);
void sub_081B6FFC(void);

void *sub_081B7064(s32 a)
{
    void *r = sub_08219FBC(9, 0x11c4);
    if (r != 0) {
        sub_0821A04C(r, sub_081B6FA8, sub_081B6FFC);
        if (sub_081B7038(r, a) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
