#include "player/player.h"
#include "player/cutscene/player_cue_sequence.h"

s32 Player_ConsumeStateEntry(struct Player *);

void sub_081481E4(struct Player *p)
{
    Player_ConsumeStateEntry(p);
    p->statusFlags &= ~1;
    Anim_SetSequence(&p->cueCollider.sequence, p->cueCollider.setupResource, 0, p->heading,
                     p->side);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->secondaryDrawFlags,
                           &p->cueCollider.sequence);
}
