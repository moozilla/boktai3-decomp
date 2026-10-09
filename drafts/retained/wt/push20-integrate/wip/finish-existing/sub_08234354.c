#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_082342AC(void *, s32, s32, s32);
void sub_0821A0C0(void *);
void sub_082340A0(void);
void sub_082340B4(void);

void *sub_08234354(s32 a, s32 b, s32 c)
{
    void *r = sub_08219FBC(8, 0x1e0);
    if (r != 0) {
        sub_0821A04C(r, sub_082340A0, sub_082340B4);
        if (sub_082342AC(r, a, b, c) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
