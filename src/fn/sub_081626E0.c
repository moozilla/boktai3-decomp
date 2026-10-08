#include "global.h"

extern void *gUnk_0200059C;
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_081625C0(void *, void *, void *, void *);
void sub_0821A0C0(void *);
void sub_0816233C(void);
void sub_08162598(void);

void *sub_081626E0(void *a, void *b, void *c)
{
    void *r;
    if (gUnk_0200059C != 0)
        return gUnk_0200059C;
    r = sub_08219FBC(0xa, 0x5A8);
    if (r != 0) {
        sub_0821A04C(r, sub_0816233C, sub_08162598);
        if (sub_081625C0(r, a, b, c) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    gUnk_0200059C = r;
    return r;
}
