#include "global.h"
#include "game_state.h"
#include "area_effect.h"
struct S {
    u8 f[0x868];
    u32 fl;
    u8 g[0xa];
    s16 v;
};
extern u16 gUnk_02000528;
s32 AreaEffect_FindContainingEntry(u8 *);
static inline u32 mk(u32 *a, u32 m)
{
    return *a & m;
}
static inline u8 tst(u32 v)
{
    if (v)
        return TRUE;
    return FALSE;
}
static inline u8 eq1(u16 v)
{
    if (v == 1)
        return TRUE;
    return FALSE;
}
s32 AreaEffect_TestPlayerCoverage(u8 *a)
{
    struct S *s = gGameState;
    if (s->v == 0)
        goto no;
    {
        u32 m = 2;
        if (tst(mk(&s->fl, m)) != 0)
            goto yes;
    }
    if (gAreaEffectManager == 0)
        goto no;
    if (AreaEffect_FindContainingEntry(a + 0x30) >= 0)
        goto yes;
    {
        u32 v = gUnk_02000528;
        u32 r = 0;
        if (v == 1)
            r = 1;
        if (r != 0)
            goto yes;
    }
no:
    return 0;
yes:
    return 1;
}
