#include "global.h"
struct P { u8 f0[0xADC]; u8 g[0xB18-0xADC]; u32 v; };
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_0818A820(void);
void sub_0818ADB4(void);
s32 sub_0818C0BC(void *, s32, s32);
void sub_08020CD4(void *, s32, s32);
struct P *sub_0818C674(u16 a, u16 b)
{
    struct P *p = sub_08219FBC(8, 0x1134);
    if (p != 0) {
        u32 *q = &p->v;
        *q = a;
        sub_0821A04C(p, sub_0818A820, sub_0818ADB4);
        if (sub_0818C0BC(p, a, b) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
        sub_08020CD4(p->g, *q, 3);
    }
    return p;
}
