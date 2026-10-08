#include "global.h"

struct S { u8 f[0x18]; s8 a; u8 g[0x43]; u32 b; u32 c; };
extern struct S *gUnk_0200002C;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u32 sub_08227E90(void);
u32 sub_0821ABA8(u32, u32);
void sub_08006A04(void *, u32);
void sub_080063D0(void *, s32);
void sub_08006738(void *);
void sub_08006E98(void)
{
    struct S *p = gUnk_0200002C;
    if (p != 0 && Script_SeekToKeyword(0x72)) {
        p->b = sub_08227E90();
        p->c = sub_0821ABA8(0x69, 0);
        if (Script_SeekToKeyword(0x77))
            p->a = Script_GetValue();
        if (p->b != 0) {
            sub_08006A04(p, p->b);
            sub_080063D0(p, p->a);
            sub_08006738(p);
        }
    }
}
