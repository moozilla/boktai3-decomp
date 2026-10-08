#include "global.h"
struct S { u8 f[0x868]; u32 fl; u8 g[0xa]; s16 v; };
extern struct S *gUnk_02000710;
extern u8 *gUnk_0200010C;
extern u16 gUnk_02000528;
s32 sub_0804E030(u8 *);
static inline u32 mk(u32 *a, u32 m) { return *a & m; }
static inline u8 tst(u32 v) { if (v) return TRUE; return FALSE; }
static inline u8 eq1(u16 v) { if (v == 1) return TRUE; return FALSE; }
s32 sub_0804E10C(u8 *a)
{
    struct S *s = gUnk_02000710;
    if (s->v == 0) goto no;
    {
        u32 m = 2;
        if (tst(mk(&s->fl, m)) != 0) goto yes;
    }
    if (gUnk_0200010C == 0) goto no;
    if (sub_0804E030(a + 0x30) >= 0) goto yes;
    {
        u32 v = gUnk_02000528;
        u32 r = 0;
        if (v == 1) r = 1;
        if (r != 0) goto yes;
    }
no:
    return 0;
yes:
    return 1;
}
