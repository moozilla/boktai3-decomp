#include "player_cue.h"
#include "player_cue_sequence.h"

void Player_SetTetherMode(struct Player *, s32, s32);

void PlayerCue_RestartAlternateAnimation(struct Player *p)
{
    Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, PlayerCue_Graphics(p)->alternateResource,
                     PlayerCue_Graphics(p)->sequenceSelection, 0, 0);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &PlayerCue_Graphics(p)->sequence);
}
