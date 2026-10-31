#include "player_cue.h"

void PlayerCue_BeginAcknowledgement(struct Player *p)
{
    PlayerCue_Messages(p)->completionKind = 2;
    PlayerCue_Messages(p)->acknowledgementPending = 1;
}
