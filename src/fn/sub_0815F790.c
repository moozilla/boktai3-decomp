#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_0815F77C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_0815F76C(void);
void sub_0815F770(void);
extern void *gUnk_02000598;

void *sub_0815F790(void *a, void *b)
{
    void *r = gUnk_02000598;
    void *q;
    if (r == 0) {
        q = sub_08219FBC(2, 0x20);
        if (q != 0) {
            sub_0821A04C(q, sub_0815F76C, sub_0815F770);
            if (sub_0815F77C(q, a, b) < 0) {
                sub_0821A0C0(q);
                return 0;
            }
        }
        r = q;
    }
    return r;
}
