#include "player/player.h"
#include "player/cutscene/player_cue_sequence.h"

s32 Player_ConsumeStateEntry(struct Player *);

void sub_08148E4C(struct Player *p)
{
    p->statusFlags &= ~1;
    Player_ConsumeStateEntry(p);
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.sideResource, 0, p->heading, p->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->secondaryDrawFlags,
                           &p->cueCollider.sequence);
}
