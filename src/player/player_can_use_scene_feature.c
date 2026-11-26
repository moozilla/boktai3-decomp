#include "engine/shared/global.h"
#include "player/player.h"
#include "system/game_state/game_state.h"
#include "player/player_slot_status.h"
#include "engine/shared/game_state_flags.h"
s32 AreaEffect_IsPositionCovered(void *);
s32 Player_CanUseSceneFeature(struct Player *p)
{
    if (!GameState_TestWorldFlags(0x1000))
        return 0;
    if (p && Player_ReadStatusBits(&p->statBonusFlags, 0x800))
        return 0;
    if (!GameState_TestWorldFlags(2) && !AreaEffect_IsPositionCovered(&p->position))
        return 0;
    return 1;
}
