#include "player_cue.h"
#include "player_cue_sequence.h"

void Player_SetTetherMode(struct Player *, s32, s32);

void Player_RestartAnimationFromResource(struct Player *p)
{
    Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, PlayerCue_Graphics(p)->sideResource, 0,
                     p->heading, p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &PlayerCue_Graphics(p)->sequence);
}
