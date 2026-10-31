#include "player_cue.h"

void PlayerCue_OverlaySet(struct Player *p, s32 v)
{
    PlayerCue_Motion(p)->overlayMode = 1;
    p->actionEighteenParameter = v;
}
