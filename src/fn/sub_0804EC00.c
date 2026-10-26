#include "global.h"
#include "game_state.h"
struct S {
    u8 f[0x868];
    u32 fl;
};
extern u8 *gUnk_020004B4;
s32 AreaEffect_FindContainingEntry(u8 *);
s32 AreaEffect_ScaleLinkResourceGain(s32);
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
static inline u16 g62(u8 *p)
{
    if (p == 0)
        return 0;
    return *(u16 *)(p + 0x62);
}
s32 AreaEffect_ScaleLinkGainAtPosition(u8 *a, s32 gain)
{
    if (g62(gUnk_020004B4) == 0)
        goto no;
    {
        u32 m = 2;
        struct S *g = gGameState;
        if (tst(mk(&g->fl, m)) != 0)
            goto yes;
    }
    if (AreaEffect_FindContainingEntry(a) >= 0)
        goto yes;
no:
    return 0;
yes:
    return AreaEffect_ScaleLinkResourceGain(gain);
}
