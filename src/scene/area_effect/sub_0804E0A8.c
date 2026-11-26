#include "global.h"
#include "system/game_state/game_state.h"
#include "scene/area_effect/area_effect.h"
s32 AreaEffect_FindContainingEntry(struct SceneVec *);
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
s32 AreaEffect_ScaleGainAtPosition(struct SceneVec *position, s32 amount)
{
    struct GameState *s = gGameState;
    if (s->unk876 == 0)
        goto no;
    {
        u32 m = 2;
        if (tst(mk(&s->flags868, m)) != 0)
            goto yes;
    }
    if (AreaEffect_FindContainingEntry(position) >= 0)
        goto yes;
    if (gAreaEffectManager->override != 0)
        goto yes;
no:
    return 0;
yes:
    return AreaEffect_ScaleResourceGain(amount);
}
