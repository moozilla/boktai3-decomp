#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08237E40(void *, s32, s32, s32);
void sub_0821A0C0(void *);
void sub_08237D68(void);
void sub_08237D7C(void);

void *sub_08237E7C(s32 a, s32 b, s32 c)
{
    void *r = sub_08219FBC(8, 0x140);
    if (r != 0) {
        sub_0821A04C(r, sub_08237D68, sub_08237D7C);
        if (sub_08237E40(r, a, b, c) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
