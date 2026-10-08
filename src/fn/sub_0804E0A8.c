#include "global.h"
struct S { u8 f[0x868]; u32 fl; u8 g[0xa]; s16 v; };
extern struct S *gUnk_02000710;
extern u8 *gUnk_0200010C;
s32 sub_0804E030(u8 *);
s32 sub_0804E008(s32);
static inline u32 mk(u32 *a, u32 m) { return *a & m; }
static inline u8 tst(u32 v) { if (v) return TRUE; return FALSE; }
s32 sub_0804E0A8(u8 *a, s32 b)
{
    struct S *s = gUnk_02000710;
    if (s->v == 0) goto no;
    {
        u32 m = 2;
        if (tst(mk(&s->fl, m)) != 0) goto yes;
    }
    if (sub_0804E030(a) >= 0) goto yes;
    if (gUnk_0200010C[0x29] != 0) goto yes;
no:
    return 0;
yes:
    return sub_0804E008(b);
}
