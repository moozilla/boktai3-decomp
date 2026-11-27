#include "minigames/coffin_bike/coffin_bike.h"
#include "system/game_state/game_state.h"

void Player_SaveCurrentHealth(void)
{
    if (gParticipantActors[0]) {
        struct CoffinBike *bike = (struct CoffinBike *)gParticipantActors[0];
        s16 *savedLife = &gGameState->unk758;
        *savedLife = bike->motion.style.initialWord;
    }
}
