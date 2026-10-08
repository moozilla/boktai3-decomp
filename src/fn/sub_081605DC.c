#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_081604F8(void *, void *, void *, void *);
void sub_0821A0C0(void *);
void sub_081604C0(void);
void sub_081604EC(void);

void *sub_081605DC(void *a, void *b, void *c)
{
    void *r = sub_08219FBC(5, 0x78);
    if (r != 0) {
        sub_0821A04C(r, sub_081604C0, sub_081604EC);
        if (sub_081604F8(r, a, b, c) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
