#include "global.h"
#include "engine/ram.h"
#include "system/game_state/game_state.h"
s32 AreaEffect_IsPositionCovered(void *);
s32 Hresvelgr_GetEnvironmentalLevel(u8 *p)
{
    s32 r;
    if (gPlayerEffectPendingCount != 0) {
        r = 4;
    } else if (AreaEffect_IsPositionCovered(p + 0x54) == 0) {
        r = 0;
    } else {
        r = gGameState->unk876;
    }
    return r;
}
