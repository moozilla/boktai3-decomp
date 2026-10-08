#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0819A5AC(void *);
void sub_0821A0C0(void *);
void sub_0819A564(void);
void sub_0819A59C(void);

void *sub_0819A65C(void)
{
    void *r = sub_08219FBC(8, 0x7c);
    if (r != 0) {
        sub_0821A04C(r, sub_0819A564, sub_0819A59C);
        if (sub_0819A5AC(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
