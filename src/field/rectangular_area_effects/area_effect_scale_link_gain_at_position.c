#include "engine/shared/global.h"
#include "system/game_state/game_state.h"
#include "engine/shared/area_effect_link.h"
s32 AreaEffect_FindContainingEntry(struct SceneVec *);
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
static inline u16 AreaEffect_GetLinkResourceValue(const struct AreaEffectLinkState *link)
{
    if (link == 0)
        return 0;
    return link->value;
}
s32 AreaEffect_ScaleLinkGainAtPosition(struct SceneVec *position, s32 gain)
{
    if (AreaEffect_GetLinkResourceValue(gLinkSession) == 0)
        goto no;
    {
        u32 m = 2;
        struct GameState *g = gGameState;
        if (tst(mk(&g->flags868, m)) != 0)
            goto yes;
    }
    if (AreaEffect_FindContainingEntry(position) >= 0)
        goto yes;
no:
    return 0;
yes:
    return AreaEffect_ScaleLinkResourceGain(gain);
}
