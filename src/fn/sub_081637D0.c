#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_08163700(void *);
void sub_0821A0C0(void *);
void sub_081636A0(void);
void sub_081636C0(void);
extern void *gUnk_02000210;

void *sub_081637D0(void)
{
    void *r = gUnk_02000210;
    void *q;
    if (r == 0) {
        q = sub_08219FBC(0xb, 0x354);
        if (q != 0) {
            sub_0821A04C(q, sub_081636A0, sub_081636C0);
            if (sub_08163700(q) < 0) {
                sub_0821A0C0(q);
                return 0;
            }
        }
        r = q;
    }
    return r;
}
