#include "global.h"
struct S { u8 f[0x868]; u32 fl; };
extern struct S *gUnk_02000710;
extern u8 *gUnk_020004B4;
s32 sub_0804E030(u8 *);
s32 sub_0804EBD8(u8 *);
static inline u32 mk(u32 *a, u32 m) { return *a & m; }
static inline u8 tst(u32 v) { if (v) return TRUE; return FALSE; }
static inline u16 g62(u8 *p) { if (p == 0) return 0; return *(u16 *)(p + 0x62); }
s32 sub_0804EC00(u8 *a, u8 *b)
{
    if (g62(gUnk_020004B4) == 0) goto no;
    {
        u32 m = 2;
        struct S *g = gUnk_02000710;
        if (tst(mk(&g->fl, m)) != 0) goto yes;
    }
    if (sub_0804E030(a) >= 0) goto yes;
no:
    return 0;
yes:
    return sub_0804EBD8(b);
}
