#include "global.h"
#include "system/game_state/game_state.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "player/player.h"
#include "player/player_slot_status.h"
s32 Solar_GetLevelForIntensity(s32);
s32 Solar_GetLevelUpperBound(s32);
s32 Solar_GetLevelLowerBound(s32);
static inline s32 modeIs(s32 m)
{
    u16 mode = gPlayerActionPending;
    s32 r = 0;
    if (mode == m)
        r = 1;
    return r;
}
s32 Solar_ApplyIntensityModifiers(s32 v)
{
    if (gEngineFlags & 0x800)
        return v;
    if (gPlayers[0]) {
        if (Player_ReadStatusBits(&gPlayers[0]->statBonusFlags, 0x40000)) {
            s32 band = Solar_GetLevelForIntensity(v) * 2;
            if (band > 10)
                band = 10;
            v = Solar_GetLevelUpperBound(band);
        }
    }
    if (gGameState->unk610) {
        if (Solar_GetLevelForIntensity(v) <= 1)
            v = Solar_GetLevelLowerBound(2);
    } else if (gGameState->unk612) {
        if (Solar_GetLevelForIntensity(v) > 2)
            v = Solar_GetLevelUpperBound(2);
    }
    if (modeIs(1)) {
        s32 band = Solar_GetLevelForIntensity(v) + 4;
        if (band > 10)
            band = 10;
        v = Solar_GetLevelUpperBound(band);
    } else if (modeIs(2))
        v = 0;
    return v;
}
