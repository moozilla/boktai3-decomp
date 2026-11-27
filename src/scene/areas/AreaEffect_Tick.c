#include "global.h"
#include "system/game_state/game_state.h"
#include "engine/ram.h"
#include "player/player.h"
#include "scene/areas/area_effect.h"
s32 AreaEffect_TestParticleMode(struct AreaEffectManager *);
void AreaEffect_UpdateActionParticles(struct AreaEffectManager *);
void AreaEffect_HideSprites(struct AreaEffectManager *);
void AreaEffect_UpdateResourceFromPlayerFlags(struct AreaEffectManager *);
void AreaEffect_ShowEnabledSprites(struct AreaEffectManager *);
s32 AreaEffect_Tick(struct AreaEffectManager *o)
{
    if (o->override != 0)
        o->override = 0;
    o->player = gPlayers[0];
    if (o->player != 0) {
        struct GameState *s = gGameState;
        if (s->unk874 == 0) {
            if (AreaEffect_TestParticleMode(o) != 0)
                AreaEffect_UpdateActionParticles(o);
            else
                AreaEffect_HideSprites(o);
        } else {
            if (s->flags868 & 2)
                AreaEffect_UpdateResourceFromPlayerFlags(o);
            else
                AreaEffect_ShowEnabledSprites(o);
        }
        {
            u32 v = gPlayerActionPending;
            u32 r = 0;
            if (v == 1)
                r = 1;
            if (r != 0 && gPlayerEffectPendingCount != 0)
                o->override = 1;
        }
    }
    return 0;
}
