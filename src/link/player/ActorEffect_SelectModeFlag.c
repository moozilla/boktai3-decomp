#include "global.h"
#include "link/player/link_player_palette.h"
#include "system/game_state/game_state.h"

static inline u8 IsFeatureEnabled(void)
{
    u32 mask = 2;
    if (gGameState->flags868 & mask)
        return TRUE;
    return FALSE;
}

s32 ActorEffect_SelectModeFlag(struct LinkPlayerFade *p)
{
    s32 result = 0;

    if (!IsFeatureEnabled()) {
        if (p->statusFlags & 16)
            result = 4;
    } else {
        if (p->statusFlags & 16)
            result = 8;
    }
    return result;
}
