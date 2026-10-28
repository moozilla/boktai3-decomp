#include "global.h"
#include "game_state.h"
#include "area_effect.h"
#include "area_effect_link.h"
struct S {
    u8 f[0x868];
    u32 fl;
};
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
static inline u8 gb(u8 *p, s32 i)
{
    if (p == 0 || i < 0)
        return 0;
    {
        u8 *q = p + 0x5c;
        return *(u8 *)((u32)q + i);
    }
}
s32 AreaEffect_IsLinkPlayerCovered(u8 *a)
{
    s32 i = *(s32 *)(a + 0x18);
    if (gb(gLinkSession, i) == 0)
        goto no;
    {
        u32 m = 2;
        struct S *g = gGameState;
        if (tst(mk(&g->fl, m)) != 0)
            goto yes;
    }
    if (gAreaEffectManager == 0)
        goto no;
    if (AreaEffect_FindContainingEntry(a + 0x30) >= 0)
        goto yes;
no:
    return 0;
yes:
    return 1;
}
