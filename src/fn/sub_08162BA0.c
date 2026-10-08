#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 sub_08162A50(void *, void *);
void sub_0821A0C0(void *);
void sub_081627A4(void);
void sub_08162A30(void);

void *sub_08162BA0(void *a)
{
    void *r = sub_08219FBC(8, 0x150);
    if (r != 0) {
        sub_0821A04C(r, sub_081627A4, sub_08162A30);
        if (sub_08162A50(r, a) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
