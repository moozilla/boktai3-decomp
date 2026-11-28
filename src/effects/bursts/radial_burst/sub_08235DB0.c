#include "effects/bursts/radial_burst.h"
#include "player/player.h"
#include "player/player_timed_cue.h"
#include "system/game_state/game_state.h"

void sub_08235DB0(struct RadialBurst *burst)
{
    burst->player->currentStatB = 0;
    if (burst->kind == 0) {
        gGameState->unk2c = 0;
        Player_SetTimedCue(1, 0x708);
        gPlayerEffectPendingCount++;
        burst->started = 1;
    } else {
        gGameState->alternateStatB = 0;
        Player_SetTimedCue(2, 0x708);
    }
}
