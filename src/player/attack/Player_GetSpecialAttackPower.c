#include "global.h"
#include "player/player.h"
#include "system/game_state/game_state.h"

extern const struct PlayerIndexedAction gUnk_08E60F4C[];

s32 Player_GetSpecialAttackPower(s32 unused, s32 idx)
{
    return gUnk_08E60F4C[idx].duration + gGameState->level;
}
#define Player_CalculateAlternateActionStat Player_GetSpecialAttackPower
