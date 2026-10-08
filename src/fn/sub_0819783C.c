#include "global.h"
struct P { u8 f[0x4c]; u32 w; };
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_0819736C(void);
void sub_0819744C(void);
void sub_081975C8(void *);
s32 sub_081976E4(void *, s32, s32);
struct P *sub_0819783C(u16 a, u16 b)
{
    struct P *p = sub_08219FBC(8, 0x72FC);
    if (p != 0) {
        p->w = a;
        sub_0821A04C(p, sub_0819736C, sub_0819744C);
        sub_081975C8(p);
        if (sub_081976E4(p, a, b) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
