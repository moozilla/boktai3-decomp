#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_080575A8(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_08057574(void);
void sub_0805759C(void);
extern void *gUnk_0200049C;

void *sub_080575F8(void *a, void *b)
{
    void *r = gUnk_0200049C;
    void *q;
    if (r == 0) {
        q = sub_08219FBC(8, 0x20);
        if (q != 0) {
            sub_0821A04C(q, sub_08057574, sub_0805759C);
            if (sub_080575A8(q, a, b) < 0) {
                sub_0821A0C0(q);
                return 0;
            }
        }
        r = q;
    }
    return r;
}
