#include "minigames/coffin_bike/coffin_bike.h"
#include "system/game_state/game_state.h"

void Character_SetLowResourceColorEffect(void *, void *);

void Player_RestoreCurrentHealth(void)
{
    struct CoffinBike *bike = (struct CoffinBike *)gParticipantActors[0];
    if (bike) {
        bike->motion.style.initialWord = gGameState->unk758;
        Character_SetLowResourceColorEffect(&bike->motion.emitter, &bike->motion.style);
    }
}
