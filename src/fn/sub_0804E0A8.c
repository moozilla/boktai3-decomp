#include "global.h"
#include "game_state.h"
struct S {
    u8 f[0x868];
    u32 fl;
    u8 g[0xa];
    s16 v;
};
extern u8 *gUnk_0200010C;
s32 AreaEffect_FindContainingEntry(u8 *);
s32 AreaEffect_ScaleResourceGain(s32);
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
s32 AreaEffect_ScaleGainAtPosition(u8 *a, s32 b)
{
    struct S *s = gGameState;
    if (s->v == 0)
        goto no;
    {
        u32 m = 2;
        if (tst(mk(&s->fl, m)) != 0)
            goto yes;
    }
    if (AreaEffect_FindContainingEntry(a) >= 0)
        goto yes;
    if (gUnk_0200010C[0x29] != 0)
        goto yes;
no:
    return 0;
yes:
    return AreaEffect_ScaleResourceGain(b);
}
