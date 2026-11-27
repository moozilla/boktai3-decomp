#include "global.h"
#include "system/game_state/game_state.h"
#include "scene/areas/area_effect.h"
#include "scene/areas/area_effect_link.h"
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
static inline u8 AreaEffect_GetLinkedPlayerValue(const struct AreaEffectLinkState *link, s32 i)
{
    if (link == 0 || i < 0)
        return 0;
    return link->playerValues[i];
}
s32 AreaEffect_IsLinkPlayerCovered(struct Player *player)
{
    s32 i = player->slot;
    if (AreaEffect_GetLinkedPlayerValue(gLinkSession, i) == 0)
        goto no;
    {
        u32 m = 2;
        struct GameState *g = gGameState;
        if (tst(mk(&g->flags868, m)) != 0)
            goto yes;
    }
    if (gAreaEffectManager == 0)
        goto no;
    if (AreaEffect_FindContainingEntry(&player->position) >= 0)
        goto yes;
no:
    return 0;
yes:
    return 1;
}
