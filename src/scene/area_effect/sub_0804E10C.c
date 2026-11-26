#include "global.h"
#include "system/game_state/game_state.h"
#include "scene/area_effect/area_effect.h"
#include "engine/ram.h"
s32 AreaEffect_FindContainingEntry(struct SceneVec *);
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
s32 AreaEffect_TestPlayerCoverage(struct Player *player)
{
    struct GameState *s = gGameState;
    if (s->unk876 == 0)
        goto no;
    {
        u32 m = 2;
        if (tst(mk(&s->flags868, m)) != 0)
            goto yes;
    }
    if (gAreaEffectManager == 0)
        goto no;
    if (AreaEffect_FindContainingEntry(&player->position) >= 0)
        goto yes;
    {
        u32 v = gPlayerActionPending;
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
