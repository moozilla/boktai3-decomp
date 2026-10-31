#include "player_cue.h"

void PlayerCue_OverlayOff(struct Player *p)
{
    PlayerCue_Motion(p)->overlayMode = 0;
}
