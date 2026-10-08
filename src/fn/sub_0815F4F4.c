#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0815F2C8(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_0815F0D0(void);
void sub_0815F224(void);

void *sub_0815F4F4(void *a, void *b)
{
    void *r = sub_08219FBC(6, 0xCAC);
    if (r != 0) {
        sub_0821A04C(r, sub_0815F0D0, sub_0815F224);
        if (sub_0815F2C8(r, a, b) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
