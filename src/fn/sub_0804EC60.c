#include "global.h"
struct S { u8 f[0x868]; u32 fl; };
extern struct S *gUnk_02000710;
extern u8 *gUnk_020004B4;
extern u8 *gUnk_0200010C;
s32 sub_0804E030(u8 *);
static inline u32 mk(u32 *a, u32 m) { return *a & m; }
static inline u8 tst(u32 v) { if (v) return TRUE; return FALSE; }
static inline u8 gb(u8 *p, s32 i) { if (p == 0 || i < 0) return 0; { u8 *q = p + 0x5c; return *(u8 *)((u32)q + i); } }
s32 sub_0804EC60(u8 *a)
{
    s32 i = *(s32 *)(a + 0x18);
    if (gb(gUnk_020004B4, i) == 0) goto no;
    {
        u32 m = 2;
        struct S *g = gUnk_02000710;
        if (tst(mk(&g->fl, m)) != 0) goto yes;
    }
    if (gUnk_0200010C == 0) goto no;
    if (sub_0804E030(a + 0x30) >= 0) goto yes;
no:
    return 0;
yes:
    return 1;
}
