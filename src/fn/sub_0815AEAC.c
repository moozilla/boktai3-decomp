#include "player_cue.h"

void PlayerCue_OverlayOn(struct Player *p, s32 v)
{
    PlayerCue_Motion(p)->overlayMode = 2;
    p->actionEighteenParameter = v;
}
