#include "player_cue.h"
#include "contact_position.h"

void PlayerCue_SetPosition(struct Player *p, const struct ContactPosition *position)
{
    PlayerCue_Coordinates(p)->position = *position;
}
